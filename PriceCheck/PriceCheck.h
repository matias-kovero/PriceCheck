#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"

#include "PriceAPI.h"

#include "version.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

/*
* !! KNOWLEDGE !!
*
* RS: https://www.youtube.com/watch?v=t-5SGaunD_s
*/

struct HandleNewOnlineItemParam
{
	void* no_touch;
	uintptr_t online_product_ptr;
};

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

	/* ITEM DROPS */
	std::list<unsigned long long> itemDrops;

	void registerCvars();
	void registerHooks();

public:
	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

	std::shared_ptr<PriceAPI> api;

	/* TRADE SPECIFIC FUNCTIONS */
	void tradeStart(TradeWrapper trade);
	void tradeEnd(TradeWrapper trade);
	void checkPrices(TradeWrapper trade);

	/* ITEM DROP FUNCTIONS */
	void getNewOnlineItem(ActorWrapper wrap, void* p);
	void showNewOnlineItem(ActorWrapper wrap, int count);
	void itemsEnded(ActorWrapper wrap);

	/* TRADEIN FUNCTIONS */
	// TODO

	/* OTHER STUFF */
	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "PriceCheck";
	void Renderer(CanvasWrapper canvas);

	// Inherited via PluginWindow
	/*

	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "PriceCheck";

	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;
	
	*/
};