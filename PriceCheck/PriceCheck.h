#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"

#include "PriceAPI.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

/*
* !! KNOWLEDGE !!
*
* Cinder: https://www.youtube.com/watch?v=jyFuN8k2uIs
* RS: https://www.youtube.com/watch?v=t-5SGaunD_s
*/

class PriceCheck: public BakkesMod::Plugin::BakkesModPlugin/*, public BakkesMod::Plugin::PluginWindow*/
{
private:
	std::shared_ptr<bool> enabled;

public:
	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

	// Custom
	std::shared_ptr<PriceAPI> api;
	Item getItem(string id);

	void logPrice(string id, CVarWrapper cvar);
	void TempTest(string id, CVarWrapper cvar);
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