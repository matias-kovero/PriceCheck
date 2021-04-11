#include "pch.h"
#include "PriceCheck.h"
#include "TradeItem.h"
#include "bakkesmod/wrappers/items/TradeWrapper.h";

#define CVAR_USE_AVG "pc_use_avg"
#define CVAR_GIVE_X "pc_give_x"
#define CVAR_GIVE_Y "pc_give_y"
#define CVAR_RECV_X "pc_recv_x"
#define CVAR_RECV_Y "pc_recv_y"
#define CVAR_HEIGHT "pc_height"
#define CVAR_WIDTH "pc_width"
#define HOOK_TRADE_START "Function TAGame.GFxData_TradeLobby_TA.HandleAcceptedInviteToTrade"
#define HOOK_TRADE_END "Function TAGame.GFxData_TradeLobby_TA.CancelTrading"
#define HOOK_TRADE_CHANGE "Function TAGame.GFxData_TradeLobby_TA.GetProductOfferings"


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
	cvarManager->registerCvar(CVAR_USE_AVG, "0", "Use AVG prices", false, true, 0, true, 1).bindTo(useAVG);

	cvarManager->registerCvar(CVAR_GIVE_X, "100", "Your items info X", false, true, 0, true, 1920).bindTo(giveX); // How to get users screen width?
	cvarManager->registerCvar(CVAR_GIVE_Y, "100", "Your items info Y", false, true, 0, true, 1080).bindTo(giveY); // How to get users screen height?
	cvarManager->registerCvar(CVAR_RECV_X, "100", "Others items info X", false, true, 0, true, 1920).bindTo(recvX); // How to get users screen width?
	cvarManager->registerCvar(CVAR_RECV_Y, "100", "Others items info Y", false, true, 0, true, 1080).bindTo(recvY); // How to get users screen height?
	
	cvarManager->registerCvar(CVAR_WIDTH, "100", "Info Width", false, true, 0, true, 400).bindTo(width);
	cvarManager->registerCvar(CVAR_HEIGHT, "100", "Info Height", false, true, 0, true, 400).bindTo(height);

	gameWrapper->RegisterDrawable(std::bind(&PriceCheck::Renderer, this, std::placeholders::_1));
	/* ==================
	*		DEBUG CVARS
	* ===================
	*/
	cvarManager->registerNotifier("priceCheck_test", std::bind(&PriceAPI::Test, api), "test", 0);

	cvarManager->registerNotifier("pc_forceupdate", std::bind(&PriceAPI::LoadData, api), "test", 0);

	cvarManager->registerNotifier("priceCheck_notif", [&](std::vector<std::string> args) {
		cvarManager->log("Hello notifier!");
		}, "", 0);

	cvarManager->registerCvar("check_price", "6", "Debug item price", true, true, 0, true, 9999, false)
		.addOnValueChanged(std::bind(&PriceCheck::logPrice, this, std::placeholders::_1, std::placeholders::_2));

	cvarManager->registerCvar("testdbg", "6", "Debug item price", true, true, 0, true, 9999, false)
		.addOnValueChanged(std::bind(&PriceCheck::TempTest, this, std::placeholders::_1, std::placeholders::_2));
}

void PriceCheck::registerHooks()
{
	gameWrapper->HookEventWithCallerPost<TradeWrapper>(HOOK_TRADE_START,
		[this](TradeWrapper caller, void* params, std::string eventName) { tradeStart(caller); });

	gameWrapper->HookEventWithCallerPost<TradeWrapper>(HOOK_TRADE_END,
		[this](TradeWrapper caller, void* params, std::string eventName) { tradeEnd(caller); });

	gameWrapper->HookEventWithCallerPost<TradeWrapper>(HOOK_TRADE_CHANGE,
		[this](TradeWrapper caller, void* params, std::string eventName) { checkPrices(caller); });
}

void PriceCheck::onLoad()
{
	_globalCvarManager = cvarManager;
	cvarManager->log("Plugin loaded!");

	api = std::make_shared<PriceAPI>(cvarManager, gameWrapper);
	_globalPriceAPI = api;

	// Get all PriceData and cache it
	api->LoadData();

	enabled = std::make_shared<bool>(true);
	useAVG = std::make_shared<bool>(false);

	giveX = std::make_shared<int>(true);
	giveY = std::make_shared<int>(true);
	recvX = std::make_shared<int>(true);
	recvY = std::make_shared<int>(true);
	width = std::make_shared<int>(true);
	height = std::make_shared<int>(true);

	tradeValueGive = std::make_shared<TradeValue>(TradeValue());
	tradeValueRecv = std::make_shared<TradeValue>(TradeValue());

	registerHooks();
	registerCvars();

	// Create more global variables, no need to load these multiple times??
	SpecialEditionDatabaseWrapper sedb = gameWrapper->GetItemsWrapper().GetSpecialEditionDB();
	_globalSpecialEditionManager = std::make_shared<SpecialEditionDatabaseWrapper>(sedb);
	
	//auto itemWrap = gameWrapper->GetItemsWrapper();
	//auto tradeWrap = itemWrap.GetTradeWrapper();
	//auto me = tradeWrap.GetSendingProducts();
	//ArrayWrapper<OnlineProductWrapper> arr = tradeWrap.GetSendingProducts();
	//ArrayWrapper<TradeItem> arr = tradeWrap.GetSendingProducts(); // no conversion on ArrayWrapper<OnlineProductWrapper> to ArrayWrapper<TradeItem>
	//TradeItem a = arr.Get(1);
	
	//TempTest();

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	//	cvarManager->log("the cvar with name: " + cvarName + " changed");
	//	cvarManager->log("the new value is:" + newCvar.getStringValue());
	//});

	//cvar2.addOnValueChanged(std::bind(&PriceCheck::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&PriceCheck::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	cvarManager->log("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&PriceCheck::YourPluginMethod, this);
}

void PriceCheck::onUnload()
{
}

Item PriceCheck::getItem(string id)
{
	auto data = api->priceData;
	auto it = data.find(id);
	if (it != data.end())
	{
		return it->second;
	}
	// Fallback, item was not found in our initially loaded data. Maybe user had connection issues.
	return api->FindItem(id);
}

void PriceCheck::logPrice(string old, CVarWrapper cvar)
{
	if (old.empty() == false && cvar.getStringValue().empty() == false)
	{
		string id = cvar.getStringValue();
		auto item = getItem(id);
		cvarManager->log("[Time] " + std::to_string(item.last_refresh));

		for (auto e : item.data) 
		{
			cvarManager->log("[Info] " + e.first + ": " + std::to_string(e.second.min) + " - " + std::to_string(e.second.max));
		}
	}
}

void PriceCheck::TempTest(string old, CVarWrapper cvar)
{
	auto items = gameWrapper->GetItemsWrapper().GetOwnedProducts();
	auto itemsCount = items.Count();
	cvarManager->log("Looks like you have " + std::to_string(itemsCount) + " items!");

	std::vector<OnlineProductWrapper> arr;
	int ic = 0;
	while (ic < itemsCount)
	{
		auto item = items.Get(ic);
		// !item.GetIsUntradable() &&
		if (!item.GetIsUntradable() && item.GetQuality() && !item.IsBlueprint() && (int)item.GetQuality() > 1 && (int)item.GetQuality() < 7) arr.push_back(item);
		ic++;
	}

	for (TradeItem item : arr)
	{
		auto name = item.GetLongLabel().ToString();
		auto price = item.GetPrice();
		if (!item.paint.empty())
			cvarManager->log(name + " [" + item.paint + "] is " + std::to_string(price.min) + " - " + std::to_string(price.max) + " credits.");
	}
}

void PriceCheck::tradeStart(TradeWrapper trade)
{
	cvarManager->log("Trade started!");
	if (trade.IsNull())
	{
		cvarManager->log("Trying to start null trade!");
		return;
	}
	*enabled = true;
}

void PriceCheck::tradeEnd(TradeWrapper trade)
{
	cvarManager->log("Trade ended!");
	if (trade.IsNull())
	{
		cvarManager->log("Trying to end null trade!");
		return;
	}
	*enabled = false;
}

void PriceCheck::checkPrices(TradeWrapper trade)
{
	// This is triggering x2 when item is changed on either offerings.
	cvarManager->log("-------- Items Changed --------");
	if (trade.IsNull())
	{
		cvarManager->log("Null trade");
		return;
	}

	ArrayWrapper<OnlineProductWrapper> me = trade.GetSendingProducts();
	ArrayWrapper<OnlineProductWrapper> him = trade.GetReceivingProducts();

	cvarManager->log("Sending (" + std::to_string(me.Count()) + "):");

	*tradeValueGive = TradeValue();
	*tradeValueRecv = TradeValue();
	/*
	tradeValueGive.operator*().max = 0;
	tradeValueRecv.operator*().min = 0;
	tradeValueRecv.operator*().max = 0;
	*/

	for (TradeItem i : me)
	{
		auto price = i.GetPrice();
		cvarManager->log(i.GetLongLabel().ToString() + " (" + i.paint + ") | " + std::to_string(price.min) + " - " + std::to_string(price.max) + " credits.");
		tradeValueGive.operator*().min += price.min;
		tradeValueGive.operator*().max += price.max;
	}
	cvarManager->log("Receiving (" + std::to_string(him.Count()) + "):");
	for (TradeItem i : him)
	{
		auto price = i.GetPrice();
		cvarManager->log(i.GetLongLabel().ToString() + " (" + i.paint + ") | " + std::to_string(price.min) + " - " + std::to_string(price.max) + " credits.");
		tradeValueRecv.operator*().min += price.min;
		tradeValueRecv.operator*().max += price.max;
	}
	cvarManager->log("---------------------------------------");
}

void PriceCheck::Renderer(CanvasWrapper canvas)
{
	if (*enabled)
	{
		Vector2 givePos = { (int)*giveX, (int)*giveY };
		Vector2 recvPos = { (int)*recvX, (int)*recvY };
		
		// Draw box
		canvas.SetColor(0, 0, 0, 55);
		canvas.SetPosition(givePos);
		canvas.FillBox(Vector2{ (int)*width, (int)*height });

		canvas.SetColor(255, 255, 255, 255);
		canvas.SetPosition(Vector2{ givePos.X + 5, givePos.Y + 5 });
		if (*useAVG)
			canvas.DrawString("Trade value: " + std::to_string((tradeValueGive.get()->min + tradeValueGive.get()->max)/2));
		else
			canvas.DrawString("Trade value: " + std::to_string(tradeValueGive.get()->min) + " - " + std::to_string(tradeValueGive.get()->max));

		canvas.SetColor(0, 0, 0, 55);
		canvas.SetPosition(recvPos);
		canvas.FillBox(Vector2{ (int)*width, (int)*height });

		canvas.SetColor(255, 255, 255, 255);
		canvas.SetPosition(Vector2{ recvPos.X + 5, recvPos.Y + 5 });
		if (*useAVG)
			canvas.DrawString("Trade value: " + std::to_string((tradeValueRecv.get()->min + tradeValueRecv.get()->max) / 2));
		else
			canvas.DrawString("Trade value: " + std::to_string(tradeValueRecv.get()->min) + " - " + std::to_string(tradeValueRecv.get()->max));

	}
}
