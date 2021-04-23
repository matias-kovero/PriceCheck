#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <httplib.h>

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "PriceAPI.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>

#include "imgui/imgui.h"

#include "fmt/core.h"
#include "fmt/ranges.h"

extern std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
extern std::shared_ptr<PriceAPI> _globalPriceAPI;
extern std::shared_ptr<SpecialEditionDatabaseWrapper> _globalSpecialEditionManager;

struct DropParams
{
  unsigned char padding[0x8];
  int ReturnValue;
};

template<typename S, typename... Args>
void LOG(const S& format_str, Args&&... args)
{
	_globalCvarManager->log(fmt::format(format_str, args...));
}

template <typename T, typename std::enable_if<std::is_base_of<ObjectWrapper, T>::value>::type*>
void GameWrapper::HookEventWithCallerPost(std::string eventName,
  std::function<void(T caller, void* params, std::string eventName)> callback)
{
  auto wrapped_callback = [callback](ActorWrapper caller, void* params, std::string eventName)
  {
    callback(T(caller.memory_address), params, eventName);
  };
  HookEventWithCaller<ActorWrapper>(eventName, wrapped_callback);
}