#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "version.h"

#include "classes/TradeIn.h"
#include "classes/PlayerTrade.h"
#include "classes/HoverItem.h"
#include "classes/ItemSeries.h"
#include "gui/Fonts.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

struct GuiState
{
	bool showTrade = false;
	bool showTradeIn = false;
	bool showInventory = false;
};

class PriceCheck: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginWindow
{
private:
	// Params to keep track of actions
	//std::shared_ptr<bool> gettingNewItems;

	/* SET FILE */
	std::shared_ptr<bool> useAVG;
	std::shared_ptr<bool> forceShow;
	std::shared_ptr<string> dataProvider;

	GuiState guiState;

	/* TRADE */
	bool showTrade = false;
	PlayerTrade playerTrade = PlayerTrade();

	/* TRADE-IN */
	bool showTradeIn = false;
	TradeIn tradeIn = TradeIn();
	ItemSeriesDatabaseWrapper itemSeriesDatabaseWrapper = ItemSeriesDatabaseWrapper();

	/* ITEM DROPS */
	std::list<unsigned long long> itemDrops;

	/* INVETORY ITEM */
	bool showInventory = false;
	HoverItem hoverItem = HoverItem();

	void registerCvars();
	void registerHooks();

	void StartRender();
	void StopRender();

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
	void checkSeriesItems(string cvarName, CVarWrapper newCvar);

	/* INVETORY ITEM */
	void showInvetoryItem(OnlineProductWrapper wrap);

	// Inherited via PluginWindow
	
	/* IMGUI STUFF */
	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "PriceCheck";
	Fonts fonts = Fonts();

	virtual void DrawTradeWindow();
	virtual void DrawTradeInWindow();

	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;

	virtual void OnOpen() override;
	virtual void OnClose() override;
};