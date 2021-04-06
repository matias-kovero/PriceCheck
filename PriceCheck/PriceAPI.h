#pragma once
#include <string>
#include <vector>

#include "bakkesmod/wrappers/cvarmanagerwrapper.h"
#include "bakkesmod/wrappers/gamewrapper.h"

#include "nlohmann/json.hpp"
#include "httplib.h"

using json = nlohmann::json;
using string = std::string;
using thread = std::thread;

/* ==================
*				Structs
* ===================
*/

struct PaintPrice
{
	int min = 0;
	int max = 0;
};

struct PaintTypes
{
	PaintPrice white;
	PaintPrice grey;
	PaintPrice crimsom;
	PaintPrice pink;
	PaintPrice cobalt;
	PaintPrice blue;
	PaintPrice sienna;
	PaintPrice saffron;
	PaintPrice lime;
	PaintPrice green;
	PaintPrice orange;
	PaintPrice purple;
};

struct Item
{
	string id;
	bool isError = false;
	string last_refresh;
	std::map<string, PaintPrice> data;
};

struct APIData
{
	int count;
	string last_refresh;
	bool isError;
	std::map<string, Item> data;
};

struct Info
{
	string title;
	string paint;
	string certified;
	string specialEdition;
};

void from_json(const json j, Item& p);
void from_json(const json j, PaintTypes& p);
void from_json(const json j, PaintPrice& p);
void from_json(const json j, APIData& p);

class PriceAPI
{
public:
	PriceAPI(std::shared_ptr<CVarManagerWrapper> _cvar, std::shared_ptr<GameWrapper> _gw);

	void Test();

	// Load all data from backend.
	//void GetAllPriceData();
	//std::map<string, Item> allPriceData;

	// Load all data from backend.
	void LoadData();
	std::map<string, Item> priceData;

	// Trigger an query to specific item.
	Item FetchItem(string id);

	// Returns give id price info.
	// Item GetCachedPriceData(string id);

private:
	std::map<string, Item> _priceData;
	std::shared_ptr<CVarManagerWrapper> _cvar;
	std::shared_ptr<GameWrapper> _gw;
	httplib::SSLClient cli;

	Item GetTemporaryPriceData(string id);

	// Callback when we get bulk data from backend
	// void OnAllPriceData(APIData res);

	// Callback to fill cache
	void OnPriceData(Item data);

	// Request data from backend
	void GetItemPrice(string id);

	// When initial data is loaded
	void OnLoadData(APIData data);
};
