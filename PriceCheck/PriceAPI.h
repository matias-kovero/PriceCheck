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
	std::map<string, Item> priceData;

	void Test();
	void LoadData();

	Item FindItem(string id);

private:
	std::map<string, Item> _priceData;
	std::shared_ptr<CVarManagerWrapper> _cvar;
	std::shared_ptr<GameWrapper> _gw;
	httplib::SSLClient cli;

	Item CreateItem(string id);

	// Request data from backend
	void FetchItem(string id);

	// When initial data is loaded
	void OnLoadData(APIData data);

	// When backend returns item data.
	void OnFetchItem(Item item);
};
