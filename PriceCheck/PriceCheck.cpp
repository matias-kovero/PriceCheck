#include "pch.h"
#include "PriceCheck.h"


BAKKESMOD_PLUGIN(PriceCheck, "Check item prices.", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void PriceCheck::onLoad()
{
	_globalCvarManager = cvarManager;
	cvarManager->log("Plugin loaded!");

	// Create our API
	api = std::make_shared<PriceAPI>(cvarManager, gameWrapper);

	// Get all PriceData and cache it
	api->LoadData();

	cvarManager->registerNotifier("priceCheck_test", std::bind(&PriceAPI::Test, api), "test", 0);

	cvarManager->registerNotifier("priceCheck_notif", [&](std::vector<std::string> args) {
		cvarManager->log("Hello notifier!");
	}, "", 0);

	cvarManager->registerCvar("check_price", "6", "Debug item price", true, true, 0, true, 9999, false)
		.addOnValueChanged(std::bind(&PriceCheck::logPrice, this, std::placeholders::_1, std::placeholders::_2));

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

		for (auto e : item.data) 
		{
			cvarManager->log("[Info] " + e.first + ": " + std::to_string(e.second.min) + " - " + std::to_string(e.second.max));
		}
	}
}