#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
//#include <Windows.h>
//#include <wincrypt.h>
#include <httplib.h>

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "PriceAPI.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include "imgui/imgui.h"

#include "fmt/core.h"
#include "fmt/ranges.h"

extern std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
extern std::shared_ptr<PriceAPI> _globalPriceAPI;
extern std::shared_ptr<SpecialEditionDatabaseWrapper> _globalSpecialEditionManager;

template<typename S, typename... Args>
void LOG(const S& format_str, Args&&... args)
{
	_globalCvarManager->log(fmt::format(format_str, args...));
}