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
	intmax_t last_refresh;
	std::map<string, PaintPrice> data;
};

struct APIData
{
	int count;
	intmax_t last_refresh;
	bool isError;
	std::map<string, Item> data;
};

struct Info
{
	string title;
	string paint;
	string certified;
	string specialEdition;
	int paintID = 0;
	int editionID = 0;
	unsigned char quality;
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

	std::map<int, string> paintNameList = {
		{0, "default"},
		{1, "crimson"},
		{2, "lime"},
		{3, "black"},
		{4, "sky blue"},
		{5, "cobalt"},
		{6, "burnt sienna"},
		{7, "forest green"},
		{8, "purple"},
		{9, "pink"},
		{10, "orange"},
		{11, "grey"},
		{12, "titanium white"},
		{13, "saffron"},
	};

	void Test();
	void LoadData();

	Item FindItem(string id);
	Item FindItem(int id);

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
