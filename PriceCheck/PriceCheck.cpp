#include "pch.h"
#include "PriceCheck.h"
#include "TradeItem.h"
#include "bakkesmod/wrappers/items/TradeWrapper.h"
#include "bakkesmod/wrappers/items/ProductTradeInWrapper.h"
#include "defines.h"

BAKKESMOD_PLUGIN(PriceCheck, "Check item prices.", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
std::shared_ptr<SpecialEditionDatabaseWrapper> _globalSpecialEditionManager;
std::shared_ptr<PriceAPI> _globalPriceAPI;

void PriceCheck::registerCvars()
{
	/* ==================
	*		SET FILE CVARS
	* ===================
	*/

	cvarManager->registerCvar(CVAR_PROVIDER, "1", "Select data provider", false).bindTo(dataProvider);
	cvarManager->registerCvar(CVAR_DATA_STATUS, " unknown", "Placeholder for data status", false);

	cvarManager->registerCvar(CVAR_USE_AVG, "0", "Use AVG prices", false, true, 0, true, 1).bindTo(useAVG);
	cvarManager->registerCvar(CVAR_FORCE_SHOW, "0", "Show all UI elements", false, true, 0, true, 1).bindTo(forceShow);

	cvarManager->registerCvar(CVAR_GIVE_X, "0.8", "Your items info X", false, true, -1.0f, true, 1.0f).bindTo(tradeX);
	cvarManager->registerCvar(CVAR_GIVE_Y, "0.85", "Your items info Y", false, true, -1.0f, true, 1.0f).bindTo(tradeY);
	
	cvarManager->registerCvar(CVAR_WIDTH, "150", "Info Width", false, true, 0, true, 400).bindTo(width);
	cvarManager->registerCvar(CVAR_HEIGHT, "65", "Info Height", false, true, 0, true, 400).bindTo(height);

	gameWrapper->RegisterDrawable(std::bind(&PriceCheck::Renderer, this, std::placeholders::_1));
	/* ==================
	*		DEBUG CVARS
	* ===================
	*/

	/*
	cvarManager->registerCvar("check_price", "6", "Debug item price", true, true, 0, true, 9999, false)
		.addOnValueChanged(std::bind(&PriceCheck::logPrice, this, std::placeholders::_1, std::placeholders::_2));
	*/
}

void PriceCheck::registerHooks()
{
	gameWrapper->HookEventWithCallerPost<TradeWrapper>(HOOK_TRADE_START,
		[this](TradeWrapper caller, void* params, std::string eventName) { tradeStart(caller); });

	gameWrapper->HookEventWithCallerPost<TradeWrapper>(HOOK_TRADE_END,
		[this](TradeWrapper caller, void* params, std::string eventName) { tradeEnd(caller); });

	gameWrapper->HookEventWithCallerPost<TradeWrapper>(HOOK_TRADE_FORCE_END,
		[this](TradeWrapper caller, void* params, std::string eventName) { tradeEnd(caller); });

	gameWrapper->HookEventWithCallerPost<TradeWrapper>(HOOK_TRADE_CHANGE,
		[this](TradeWrapper caller, void* params, std::string eventName) { checkPrices(caller); });

	/* THESE ARE NOT YET READY FOR RELEASE, NOT HOOKING THEM YET */
	
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(HOOK_NEW_ITEM,
		[this](ActorWrapper caller, void* params, std::string eventName) { 
			cvarManager->log(eventName);
			*gettingNewItems = true;
			getNewOnlineItem(caller, params); 
		});

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(HOOK_SHOW_NEW_ITEM,
		[this](ActorWrapper caller, void* params, std::string eventName) {
			cvarManager->log(eventName);
			showNewOnlineItem(caller, 1);
		});

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(HOOK_PENDING_DROP,
		[this](ActorWrapper caller, void* params, std::string eventName) {
			if (!params) return;
			auto t_params = (DropParams*)params;
			int pending = t_params->ReturnValue;
			if (pending > 0 )
			{
				showNewOnlineItem(caller, pending);
			}
		});

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(HOOK_DROPS_ENDED,
		[this](ActorWrapper caller, void* params, std::string eventName) { itemsEnded(caller); });

	gameWrapper->HookEventWithCallerPost<ProductTradeInWrapper>(HOOK_TRADE_IN_UPDATE,
		[this](ProductTradeInWrapper caller, void* params, std::string eventName) { checkPrices(caller); });

}

void PriceCheck::onLoad()
{
	_globalCvarManager = cvarManager;

	/* ITEM API */
	api = std::make_shared<PriceAPI>(cvarManager, gameWrapper);
	_globalPriceAPI = api;
	// Should we use api or _globalPriceAPI?
	api->LoadData();

	// Values to keep track on actions
	gettingNewItems = std::make_shared<bool>(false);

	/* SET FILE PARAMS */
	dataProvider = std::make_shared<string>();
	useAVG = std::make_shared<bool>(false);
	forceShow = std::make_shared<bool>(false);

	tradeX = std::make_shared<float>(true);
	tradeY = std::make_shared<float>(true);

	width = std::make_shared<int>(true);
	height = std::make_shared<int>(true);

	/* MISC STUFF */
	registerHooks();
	registerCvars();

	/* FOR TRADEITEMS */ // Is this NONO? check TradeItem.cpp -> updateItemInfo()
	SpecialEditionDatabaseWrapper sedb = gameWrapper->GetItemsWrapper().GetSpecialEditionDB();
	_globalSpecialEditionManager = std::make_shared<SpecialEditionDatabaseWrapper>(sedb);
}

void PriceCheck::onUnload()
{
}

void PriceCheck::tradeStart(TradeWrapper trade)
{
	//cvarManager->log("Trade start");
	api->Refresh();

	if (trade.IsNull())
	{
		cvarManager->log("Trying to start null trade!");
		return;
	}
	showTrade = true;
	// Should be empty but double checking
	tradeValueGive = TradeValue();
	tradeValueRecv = TradeValue();
}

void PriceCheck::tradeEnd(TradeWrapper trade)
{
	//cvarManager->log("Trade end");
	if (trade.IsNull())
	{
		cvarManager->log("Trying to end null trade!");
		return;
	}
	showTrade = false;
	// Reset trade values
	tradeValueGive = TradeValue();
	tradeValueRecv = TradeValue();
}

void PriceCheck::checkPrices(TradeWrapper trade)
{
	if (trade.IsNull())
	{
		cvarManager->log("Null trade");
		return;
	}
	tradeValueGive = TradeValue();
	tradeValueRecv = TradeValue();

	ArrayWrapper<OnlineProductWrapper> me = trade.GetSendingProducts();
	std::vector<TradeWrapper::Currency> meCurrency = trade.GetSendingCurrency();

	for (TradeWrapper::Currency c : meCurrency)
	{
		if (c.currency_id == Currencies::Credits) 
		{
			tradeValueGive.min += c.quantity;
			tradeValueGive.max += c.quantity;
		}
	}
	
	ArrayWrapper<OnlineProductWrapper> him = trade.GetReceivingProducts();
	std::vector<TradeWrapper::Currency> himCurrency = trade.GetReceivingCurrency();

	for (TradeWrapper::Currency c : himCurrency)
	{
		if (c.currency_id == Currencies::Credits)
		{
			tradeValueRecv.min += c.quantity;
			tradeValueRecv.max += c.quantity;
		}
	}

	for (TradeItem i : me)
	{
		PaintPrice price = i.GetPrice();
		tradeValueGive.min += price.min;
		tradeValueGive.max += price.max;
	}

	for (TradeItem i : him)
	{
		PaintPrice price = i.GetPrice();
		tradeValueRecv.min += price.min;
		tradeValueRecv.max += price.max;
	}
}

void PriceCheck::getNewOnlineItem(ActorWrapper wrap, void* params)
{
	api->Refresh();

	if (!params)
	{
		cvarManager->log("Null params");
		return;
	}
	auto t_params = (HandleNewOnlineItemParam*)params;
	OnlineProductWrapper online_product{ t_params->online_product_ptr };
	if (!online_product) 
	{
		cvarManager->log("Null online_product");
		return;
	}
	// Seems its using LIFO when displaying over 1 drops
	itemDrops.push_front(online_product.GetInstanceID());
}

void PriceCheck::showNewOnlineItem(ActorWrapper wrap, int count)
{
	cvarManager->log("itemDrops: (" + std::to_string(itemDrops.size()) + "), count: (" + std::to_string(count) + ")");

	if (!itemDrops.empty() && itemDrops.size() >= count)
	{
		cvarManager->log("Vector len: (" + std::to_string(itemDrops.size()) + ")");
		unsigned long long id = itemDrops.front();
		TradeItem i = gameWrapper->GetItemsWrapper().GetOnlineProduct(id);
		if (!i) 
		{
			cvarManager->log("Trade item is null");
			return;
		}

		PaintPrice price = i.GetPrice();
		string paint = i.GetPaint();

		gameWrapper->Toast("New Item",
			i.GetLongLabel().ToString() + 
			(paint != "" ? " (" + paint + ")" : "") +
			"\n" + std::to_string(price.min) + " - " + std::to_string(price.max),
			"default", 4.5F
		);

		itemDrops.pop_front();
	}
	else {
		cvarManager->log("Seems that vector is empty.");
	}
}

void PriceCheck::itemsEnded(ActorWrapper wrap) 
{
	itemDrops.clear();
	cvarManager->log("User has seen all items.");
}

void PriceCheck::checkPrices(ProductTradeInWrapper tradeIn)
{
	if (tradeIn.IsNull())
	{
		cvarManager->log("Null trade-in");
		return;
	}
	tradeInValue = TradeValue();
	ArrayWrapper<OnlineProductWrapper> items = tradeIn.GetProducts();

	cvarManager->log("User is trading in " + std::to_string(items.Count()) + " items.");

	for (TradeItem i : items) 
	{
		PaintPrice price = i.GetPrice();
		tradeInValue.min += price.min;
		tradeInValue.max += price.max;
	}
	cvarManager->log("Trade-In valuation: " + std::to_string(tradeInValue.min) + " - " + std::to_string(tradeInValue.max));
}

void PriceCheck::Renderer(CanvasWrapper canvas)
{
	if (showTrade || *forceShow)
	{
		Vector2 screenSize = gameWrapper->GetScreenSize();

		Vector2F normPos = { 
			(*tradeX + 1.0) * (screenSize.X * 0.5 - *width / 2), 
			(-*tradeY + 1.0) * (screenSize.Y * 0.5 - *height / 2)
		};
		
		Vector2 givePos = { (int)normPos.X, (int)normPos.Y };

		// Draw box
		canvas.SetColor(0, 0, 0, 55);
		canvas.SetPosition(givePos);
		canvas.FillBox(Vector2{ (int)*width, (int)*height });

		string gMin = std::to_string(tradeValueGive.min);
		string gMax = std::to_string(tradeValueGive.max);
		string rMin = std::to_string(tradeValueRecv.min);
		string rMax = std::to_string(tradeValueRecv.max);

		int rSum = (tradeValueRecv.min + tradeValueRecv.max) / 2;
		int gSum = (tradeValueGive.min + tradeValueGive.max) / 2;

		canvas.SetColor(255, 255, 255, 255);
		canvas.SetPosition(Vector2{ givePos.X + 5, givePos.Y + 5 });

		if (*useAVG)
			canvas.DrawString("Give up: " + std::to_string(gSum));
		else
			canvas.DrawString("Give up: " + gMin + " - " + gMax);

		canvas.SetColor(255, 255, 255, 255);
		canvas.SetPosition(Vector2{ givePos.X + 5, givePos.Y + 20 });

		if (*useAVG)
			canvas.DrawString("Receive: " + std::to_string(rSum));
		else
			canvas.DrawString("Receive: " + rMin + " - " + rMax);

		canvas.SetColor(255, 255, 255, 130);
		canvas.DrawLine(
			Vector2{ givePos.X + 5, givePos.Y + 43 },
			Vector2{ givePos.X + *width - 5, givePos.Y + 43 }
		);
		string state = "Result: ";

		rSum == gSum ? canvas.SetColor(255, 255, 255, 255) :
			rSum > gSum ? canvas.SetColor(50, 168, 82, 255) :
			canvas.SetColor(168, 50, 50, 255);

		rSum == gSum ? state = "Result: " :
			rSum > gSum ? state = "Profit: " : state = "Loss: ";

		canvas.SetPosition(Vector2{ givePos.X + 5, givePos.Y + 45 });

		if (*useAVG)
			canvas.DrawString(state + std::to_string(rSum - gSum));
		else
			canvas.DrawString(state + 
				std::to_string(std::abs(tradeValueRecv.min - tradeValueGive.min)) + 
				" - " + 
				std::to_string(std::abs(tradeValueRecv.max - tradeValueGive.max)));
	}
}
