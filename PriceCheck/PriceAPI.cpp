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