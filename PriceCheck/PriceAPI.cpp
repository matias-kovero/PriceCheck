#include "pch.h"
#include "PriceAPI.h"


PriceAPI::PriceAPI(std::shared_ptr<CVarManagerWrapper> cvar, std::shared_ptr<GameWrapper> gw) : cli("bm-plugin-prices.vercel.app"), _cvar(cvar), _gw(gw)
{
	// cli.set_follow_location(true);
}

void PriceAPI::Test()
{
	_gw->Toast("PriceAPI log", "getting item price. Check console for results.");
	thread t([this]() {
		string url = "/item/";
		cli.set_follow_location(true);
		auto res = cli.Get(url.c_str());
		if (res && res->status == 200)
		{
			_cvar->log("We got stuff :D");
		}
		else _cvar->log("Res did not return 200");
		});
	t.detach();
}

/// <summary>
/// Initially load data from our backend.
/// This will fill your local data with updated price information.
/// OLD CODE: GetAllPriceData
/// </summary>
void PriceAPI::LoadData()
{
	_cvar->log("API started loading data...");
	_gw->Toast("PriceAPI log", "Fetching data...");
	thread t([this]() {
		string url = "/item/";

		cli.set_follow_location(true);

		auto res = cli.Get(url.c_str());
		if (res && res->status == 200)
		{
			json j = json::parse(res->body);
			try
			{
				auto data = j.get<APIData>();
				_cvar->log("Items are updated: " + data.last_refresh);
				_cvar->log("We have info on " + std::to_string(data.count) + " items.");
				OnLoadData(data);
			}
			catch (const std::exception& e)
			{
				_cvar->log(e.what());
			}
		}
		else _cvar->log("Brrsz. Server not alive.");
		});
	t.detach();
}
/// <summary>
/// Query singular item information from the backend.
/// </summary>
/// <param name="id"></param>
void PriceAPI::FetchItem(string id)
{
	_gw->Toast("PriceAPI log", "getting item price. Check console.");
	thread t([this, id]() {
		string url = "/item/" + id;
		cli.set_follow_location(true);
		auto res = cli.Get(url.c_str());
		if (res && res->status == 200)
		{
			json j = json::parse(res->body);
			try
			{
				auto data = j.get<Item>();
				//_cvar->log(std::to_string(data.test[1].white.min));
				if (data.isError) _cvar->log("Item (" + id + ") not found. No price info on this item, if seems odd - contact developer!");
				else 
				{
					_cvar->log("Time: " + data.last_refresh);
					//_cvar->log("Going raw: " + res->body);
					OnFetchItem(data);
				}
			}
			catch (const std::exception& e)
			{
				_cvar->log(e.what());
			}
		}
		else _cvar->log("Brrsz. Its not 200 today :(");
	});
	t.detach();
}

/// <summary>
/// Find item price information.
/// </summary>
/// <param name="id">Item ID</param>
/// <returns></returns>
Item PriceAPI::FindItem(string id)
{
	auto it = _priceData.find(id);
	if (it != _priceData.end())
	{
		_cvar->log("Found in item (" + it->second.id + ") in cache.");
		return it->second;
	}
	else
	{
		_cvar->log("Item (" + id + ") not found. Sending req...");
		_priceData[id] = CreateItem(id); // Insert?
		FetchItem(id);
	}
	return Item();
}

/// <summary>
/// Create an template for an item. It will be filled with data when query is fulfilled.
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
Item PriceAPI::CreateItem(string id)
{
	for (auto& d : priceData)
	{
		if (d.second.id == id) return d.second;
	}
	return Item();
}

/*
* =====================
*			CALLBACKS
* =====================
*/
/// <summary>
/// Callback when data is received from the backend. OLD CODE: OnAllPriceData
/// </summary>
/// <param name="res">APIData</param>
void PriceAPI::OnLoadData(APIData res)
{
	priceData = res.data;
	_cvar->log("API Data loaded!");
}

/// <summary>
/// Callback when item data is received from the backend. Update local copy with updated info.
/// </summary>
/// <param name="item">Item data</param>
void PriceAPI::OnFetchItem(Item item)
{
	_cvar->log("Updating item(" + item.id + ")");
	// Dbg log
	/*
	for (auto e : item.data)
	{
		_cvar->log("Data:" + e.first + " " + std::to_string(e.second.min) + " " + std::to_string(e.second.max));
	}
	*/

	auto it = _priceData.find(item.id);
	if (it != _priceData.end())
	{
		// Item found on cache, update it.
		_priceData[item.id] = item;
	}
	else
	{
		// Item not found in cache (rare?), add it.
		_priceData.emplace(item.id, item); // Is there an possibility on duplicates?
	}
}


/* 
* =====================
*			JSON PARSING
* =====================
*/

#define J(var) j.at(#var).get_to(p.var);
#define J2(var, var2) j.at(#var).get_to(p.var2);
#define JOPT(var) if(j.find(#var) != j.end()) {j.at(#var).get_to(p.var);}
#define JOPT2(var, var2) if(j.find(#var) != j.end()){j.at(#var).get_to(p.var2);}
#define JOPTD (var) if(j.find(#var) != j.end()) {j.at(#var).get_to(p.var);}

void from_json(const json j, Item& p)
{
	JOPT(id);
	JOPT2(error, isError);
	JOPT2(time, last_refresh);
	J(data);
}

void from_json(const json j, PaintTypes& p)
{
	JOPT2(titanium white, white);
	JOPT(grey);
	JOPT(crimsom);
	JOPT(pink);
	JOPT(cobalt);
	JOPT2(sky blue, blue);
	JOPT2(burnt sienna, sienna);
	JOPT(saffron);
	JOPT(lime);
	JOPT2(forest green, green);
	JOPT(orange);
	JOPT(purple);
}

void from_json(const json j, PaintPrice& p)
{
	JOPT(min);
	JOPT(max);
}

void from_json(const json j, APIData& p)
{
	J(count);
	J2(time, last_refresh);
	J2(error, isError);
	J(data);
	for (auto& [id, item] : p.data)
	{
		item.id = id;
		item.isError = p.isError;
		item.last_refresh = p.last_refresh;
	}
}

#undef J
#undef JOPT