#pragma once
#include <string>
#include <vector>

#include "bakkesmod/wrappers/cvarmanagerwrapper.h"
#include "bakkesmod/wrappers/gamewrapper.h"

#include "nlohmann/json.hpp"
#include "httplib.h"

#include "APIClasses.h"

using json = nlohmann::json;
using string = std::string;
using thread = std::thread;

class PriceAPI
{
public:
	PriceAPI(std::shared_ptr<CVarManagerWrapper> _cvar, std::shared_ptr<GameWrapper> _gw);
	std::map<string, Item> priceData; // Is this even mandatory??
	string lastUpdated;

	void LoadData();

	void Refresh();

	Item FindItem(string id);
	Item FindItem(int id);

	Blueprint FindBlueprint(string id);
	Blueprint FindBlueprint(int id);

private:
	std::map<string, Blueprint> _blueprintData;
	std::map<string, Item> _priceData;

	std::shared_ptr<CVarManagerWrapper> _cvar;
	std::shared_ptr<GameWrapper> _gw;
	httplib::SSLClient cli;

	intmax_t _last_refresh = 0;
	bool _fetching = false;
	int _error_count = 0;

	void SetUpParser();

	bool CheckTimeStamp(intmax_t last_update);

	// When initial data is loaded
	void OnLoadData(APIData data);

	void OnResponseError();
};
