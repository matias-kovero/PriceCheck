#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "version.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

struct TradeValue
{
	int min = 0;
	int max = 0;
};

enum Currencies
{
	Credits = 13
};

class PriceCheck: public BakkesMod::Plugin::BakkesModPlugin/*, public BakkesMod::Plugin::PluginWindow*/
{
private:
	// Params to keep track of actions
	std::shared_ptr<bool> gettingNewItems;

	/* SET FILE */
	std::shared_ptr<bool> useAVG;
	std::shared_ptr<bool> forceShow;
	std::shared_ptr<string> dataProvider;

	std::shared_ptr<float> tradeX;
	std::shared_ptr<float> tradeY;

	std::shared_ptr<int> width;
	std::shared_ptr<int> height;

	/* TRADE */
	bool showTrade = false;
	TradeValue tradeValueGive;
	TradeValue tradeValueRecv;

	/* TRADE-IN */
	bool showTradeIn = false;
	TradeValue tradeInValue;

	/* ITEM DROPS */
	std::list<unsigned long long> itemDrops;

	void registerCvars();
	void registerHooks();

public:
	virtual void onLoad();
	virtual void onUnload();

	std::shared_ptr<PriceAPI> api;

	/* TRADE FUNCTIONS */
	void tradeStart(TradeWrapper trade);
	void tradeEnd(TradeWrapper trade);
	void checkPrices(TradeWrapper trade);

	/* ITEM DROP FUNCTIONS */
	void getNewOnlineItem(ActorWrapper wrap, void* p);
	void showNewOnlineItem(ActorWrapper wrap, int count);
	void itemsEnded(ActorWrapper wrap);

	/* TRADE-IN FUNCTIONS */
	void checkPrices(ProductTradeInWrapper tradeIn);
	// TODO

	/* OTHER STUFF */
	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "PriceCheck";
	void Renderer(CanvasWrapper canvas);
};