#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "version.h"
#include "classes/TradeIn.h"
#include "classes/PlayerTrade.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class PriceCheck: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginWindow
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
	std::shared_ptr<float> tradeInX;
	std::shared_ptr<float> tradeInY;

	std::shared_ptr<int> width;
	std::shared_ptr<int> height;

	/* TRADE */
	bool showTrade = false;
	PlayerTrade playerTrade = PlayerTrade();

	/* TRADE-IN */
	bool showTradeIn = false;
	TradeIn tradeIn = TradeIn();

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
	void checkPrices(ProductTradeInWrapper wrap);
	void tradeInEnded(ProductTradeInWrapper wrap);

	/* RENDERER STUFF */
	void Renderer(CanvasWrapper canvas);
	void RenderTrade(CanvasWrapper canvas);

	// Inherited via PluginWindow
	
	/* IMGUI STUFF */
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
};