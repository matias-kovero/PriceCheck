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

	cvarManager->registerCvar(CVAR_TRADE_IN_X, "0.93", "Your trade in X", false, true, -1.0f, true, 1.0f).bindTo(tradeInX);
	cvarManager->registerCvar(CVAR_TRADE_IN_Y, "0.93", "Your trade in Y", false, true, -1.0f, true, 1.0f).bindTo(tradeInY);
	
	cvarManager->registerCvar(CVAR_WIDTH, "150", "Info Width", false, true, 0, true, 400).bindTo(width);
	cvarManager->registerCvar(CVAR_HEIGHT, "65", "Info Height", false, true, 0, true, 400).bindTo(height);

	gameWrapper->RegisterDrawable(std::bind(&PriceCheck::Renderer, this, std::placeholders::_1));
	// Why the hell it won't just work with ""
	gameWrapper->LoadToastTexture("pricecheck_logo", std::string("./bakkesmod/data/assets/pricecheck_logo.tga"));
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
	
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(HOOK_NEW_ITEM,
		[this](ActorWrapper caller, void* params, std::string eventName) { 
			*gettingNewItems = true;
			getNewOnlineItem(caller, params); 
		});

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(HOOK_SHOW_NEW_ITEM,
		[this](ActorWrapper caller, void* params, std::string eventName) {
			showNewOnlineItem(caller, 1);
		});

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(HOOK_PENDING_DROP,
		[this](ActorWrapper caller, void* params, std::string eventName) {
			if (!params) return;
			auto t_params = (DropParams*)params;
			int pendingAmount = t_params->ReturnValue;
			if (pendingAmount > 0 )
			{
				showNewOnlineItem(caller, pendingAmount);
			}
		});

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(HOOK_DROPS_ENDED,
		[this](ActorWrapper caller, void* params, std::string eventName) { itemsEnded(caller); });

	gameWrapper->HookEventWithCallerPost<ProductTradeInWrapper>(HOOK_TRADE_IN_UPDATE,
		[this](ProductTradeInWrapper caller, void* params, std::string eventName) { checkPrices(caller); });

	gameWrapper->HookEventWithCallerPost<ProductTradeInWrapper>(HOOK_TRADE_IN_CLOSE,
		[this](ProductTradeInWrapper caller, void* params, std::string eventName) { tradeInEnded(caller); });
}

void PriceCheck::onLoad()
{
	_globalCvarManager = cvarManager;
	cvarManager->setBind("F8", "togglemenu PriceCheck");

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
	tradeInX = std::make_shared<float>(true);
	tradeInY = std::make_shared<float>(true);

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
	api->Refresh();

	if (trade.IsNull())
	{
		LOG("{}: Trade is null", __FUNCTION__);
		return;
	}
	showTrade = true;
	// Should be empty but double checking
	playerTrade.Clear();
}

void PriceCheck::tradeEnd(TradeWrapper trade)
{
	if (trade.IsNull())
	{
		LOG("{}: Trade is null", __FUNCTION__);
		return;
	}
	showTrade = false;
	playerTrade.Clear();
}

void PriceCheck::checkPrices(TradeWrapper trade)
{
	if (trade.IsNull())
	{
		LOG("{}: Trade is null", __FUNCTION__);
		return;
	}
	playerTrade.CheckTrade(trade);
}

void PriceCheck::getNewOnlineItem(ActorWrapper wrap, void* params)
{
	api->Refresh();

	if (!params)
	{
		LOG("{}: Params are null", __FUNCTION__);
		return;
	}
	auto t_params = (HandleNewOnlineItemParam*)params;
	OnlineProductWrapper online_product{ t_params->online_product_ptr };
	if (!online_product) 
	{
		LOG("{}: online_product is null", __FUNCTION__);
		return;
	}
	// Big amount of debuggin' on item drops. Order of drops still unknown.
	auto pr = online_product.GetProduct();
	LOG("ID: {}, Series: {}, Quality: {}, Container: {}, Blueprint: {}, UnlockMethod: {}, Slot: {}, SortLabel: {}", pr.GetID(), online_product.GetSeriesID(), online_product.GetQuality(), pr.IsContainer(), online_product.IsBlueprint(), pr.GetUnlockMethod(), pr.GetSlot().GetSlotIndex(), pr.GetSortLabel().ToString());
	// ID: 1111, SortLabel: Octane: Toon Sketch, UnlockMethod: 1, IsContainer: false, UnlockMthd: 1, DisplayLabelSlot: Animated Decal
	/* GOT 2 DROPS: [3036, 5587] -> [3036, 5587] | push_front is not right */
	// ID: 3036, Series: 1315, Quality: 8, Container: false, Blueprint: false, UnlockMethod: 1, Slot: 21, SortLabel: Title
	// ID: 5587, Series: 1282, Quality: 8, Container: false, Blueprint: false, UnlockMethod: 1, Slot: 14, SortLabel: Pixel Pointer
	// GOT 1 DROP: [5610] -> [5610] | it was an blueprint -> Season 1 Series
	// ID: 5610, Series: 4, Quality: 3, Container: true, Blueprint: true, UnlockMethod: 1, Slot: 24, SortLabel: Season 1
	// GOT 1 DROP: [5592] -> [5592] | Quality = Limited
	// ID: 5592, Series: 1283, Quality: 8, Container: false, Blueprint: false, UnlockMethod: 1, Slot: 2, SortLabel: Imptekk
	// GOT 3 DROPS: [5144,5301,5931] -> [5931, ????, 5144] | Got only 2 drops to show. Array also had 4 items? Video: itemdrop0305
	// ID: 5144, Series: 4, Quality: 4, Container: true, Blueprint: true, UnlockMethod: 1, Slot: 24, SortLabel: Momentum
	// ID: 5301, Series: 1119, Quality: 2, Container: true, Blueprint: false, UnlockMethod: 1, Slot: 11, SortLabel: Rare Drop
	// ID: 5931, Series: 1284, Quality: 8, Container: false, Blueprint: false, UnlockMethod: 1, Slot: 1, SortLabel: Tyranno: Reviver
	// 
	// Drops: 4, count: 3
	// Vector length: 4
	// Got toast from Item Drop: Tyranno: Reviver
	// Drops: 3, count: 2
	// Vector length: 3
	// Got toast from Item Drop: Rare Drop
	// Drops: 2, count: 2
	// Got toast from Item Drop: HNY Blueprint
	// 
	// GOT DROPS 1: [5610] -> [5610] | Item Drop: Fennec: Distortion Blueprint
	// ID: 5610, Series: 4, Quality: 3, Container: true, Blueprint: true, UnlockMethod: 1, Slot: 24, SortLabel: Season 1
	// GOT DROPS 1: [4737] -> [4737] | Item Drop: Ribbon Candy
	// ID: 4737, Series: 1285, Quality: 8, Container: false, Blueprint: false, UnlockMethod: 1, Slot: 7, SortLabel: Ribbon Candy
	// GOT DROPS 1: [5301] -> [5301] | Item Drop: Ribbon Candy
	// ID: 5301, Series: 1119, Quality: 2, Container: true, Blueprint: false, UnlockMethod: 1, Slot: 11, SortLabel: Rare Drop
	// 

	// ID: 5303, Series : 1120, Quality : 3, Container : true, Blueprint : false, UnlockMethod : 1, Slot : 11, SortLabel : Very Rare Drop
	// Should add : 17939505570, itemDrops : {17939505570, 17938362897}
	// ID : 5340, Series : 1294, Quality : 8, Container : false, Blueprint : false, UnlockMethod : 1, Slot : 1, SortLabel : Retrogression
	// Should add : 17939505578, itemDrops : {17939505578, 17939505570, 17938362897}
	// New Item: Retrogression
	// New Item: Very Rare Drop
	// New Item: Peregrine TT: Crisis Blueprint
	// Maybe its highest quality, then smallest ID
	// Seems its using LIFO when displaying over 1 drops
	// Could we use map -> <ID, InstanceID>
	itemDrops.push_front(online_product.GetInstanceID());
	LOG("Should add: {}, itemDrops: {}", online_product.GetInstanceID(), itemDrops);
}

void PriceCheck::showNewOnlineItem(ActorWrapper wrap, int count)
{
	LOG("Drops: {}, caller: {}", itemDrops.size(), count);

	if (!itemDrops.empty() && itemDrops.size() >= count)
	{
		LOG("Drops amount: {}", itemDrops.size());
		unsigned long long id = itemDrops.front();
		TradeItem i = gameWrapper->GetItemsWrapper().GetOnlineProduct(id);
		if (!i) 
		{
			LOG("{}: Item is null", __FUNCTION__);
			return;
		}

		PaintPrice price = i.GetPrice();
		string paint = i.GetPaint();

		LOG("IsContainer: {}", i.GetProduct().IsContainer());
		LOG("AssetPath: {}", i.GetProduct().GetThumbnailAssetPath().ToString());
		// AssetPath: Wheel_Hanzawa_T.Wheel_Hanzawa_T
		gameWrapper->Toast(
			"New Item",
			i.GetLongLabel().ToString() + "\n" +				// Name
			(paint != "" ? "(" + paint + ")\n" : "") +	// Paint
			std::to_string(price.min) + " - " + std::to_string(price.max), // Price min - max
			"pricecheck_logo", 4.5f, ToastType_Info);

		itemDrops.pop_front();
	}
	else {
		LOG("{}: Drops seem empty", __FUNCTION__);
	}
}

void PriceCheck::itemsEnded(ActorWrapper wrap) 
{
	itemDrops.clear();
}

void PriceCheck::checkPrices(ProductTradeInWrapper wrap)
{
	if (wrap.IsNull())
	{
		LOG("{}: Trade In is null", __FUNCTION__);
		return;
	}
	ArrayWrapper<OnlineProductWrapper> items = wrap.GetProducts();
	if (items.IsNull()) 
	{
		showTradeIn = false;
		return;
	}
	showTradeIn = items.Count() > 0 ? true : false;
	// As we will loop old items as well, clear the existing list.
	tradeIn.Clear();

	for (TradeItem i : items) tradeIn.AddItem(i);
}

void PriceCheck::tradeInEnded(ProductTradeInWrapper wrap)
{
	showTradeIn = false;
	tradeIn.Clear();
}

void PriceCheck::Renderer(CanvasWrapper canvas)
{
	if (*forceShow)
	{
		Vector2 screenSize = gameWrapper->GetScreenSize();
		RenderTrade(canvas);
		tradeIn.Render(canvas, screenSize, Vector2F{ *tradeInX, *tradeInY }, *useAVG);
	}
	else if (showTrade)
	{
		RenderTrade(canvas);
	} 
	else if (showTradeIn)
	{
		Vector2 screenSize = gameWrapper->GetScreenSize();
		tradeIn.Render(canvas, screenSize, Vector2F{ *tradeInX, *tradeInY }, *useAVG);
	}
}

void PriceCheck::RenderTrade(CanvasWrapper canvas)
{
	Vector2 screenSize = gameWrapper->GetScreenSize();

	Vector2F normPos = {
		(*tradeX + 1.0) * (screenSize.X * 0.5 - *width / 2),
		(-*tradeY + 1.0) * (screenSize.Y * 0.5 - *height / 2)
	};

	playerTrade.Render(
		canvas,
		Vector2{ (int)normPos.X, (int)normPos.Y },
		Vector2{ (int)*width, (int)*height },
		*useAVG
	);
}
