#include "pch.h"
#include "defines.h"
#include "APIClasses.h"

/// <summary>
/// Initially load data from our backend.
/// This will fill your local data with updated price information.
/// </summary>
void PriceAPI::LoadData()
{
	thread t([this]() {
		string url = "/insider/pc";
		_fetching = true;
		cli.set_follow_location(true);
		auto res = cli.Get(url.c_str());
		if (res && res->status == 200)
		{
			json j = json::parse(res->body);
			try
			{
				auto data = j.get<APIData>();
				_cvar->log("Update timestamp: " + std::to_string(data.last_refresh));
				OnLoadData(data);
			}
			catch (const std::exception& e)
			{
				_fetching = false;
				_cvar->log(e.what());
			}
		}
		else 
		{
			_fetching = false;
			_cvar->log("Proxy server not responding...");
			std::this_thread::sleep_for(std::chrono::seconds(1));
			OnResponseError();
		}
	});
	t.detach();
}

/// <summary>
/// Refresh data if needed.
/// </summary>
void PriceAPI::Refresh()
{
	bool need = CheckTimeStamp(_last_refresh);
	if (need && !_fetching) LoadData();
}

Item PriceAPI::FindItem(string id)
{
	auto it = _priceData.find(id);
	if (it != _priceData.end())
	{
		return it->second;
	}
	else
	{
		//_gw->Toast("PriceCheck", "Unable to find price info for item.", "pricecheck_logo", 3.5f, ToastType_Warning);
		_priceData[id] = Item();
		return _priceData[id];
	}
	_gw->Toast("PriceCheck", "Siwat takaisin");
	return Item();
}

Item PriceAPI::FindItem(int id)
{
	return FindItem(std::to_string(id));
}

Item PriceAPI::FindBlueprint(string id)
{
	auto it = _blueprintData.find(id);
	if (it != _blueprintData.end())
	{
		return it->second;
	}
	else 
	{
		//_gw->Toast("PriceCheck", "Can't find price info for blueprint: " + id );
		_blueprintData[id] = Item();
		return _blueprintData[id];
	}
	_gw->Toast("PriceCheck", "Entäs Tarmon lähikaupat");
	return Item();
}

Item PriceAPI::FindBlueprint(int id)
{
	return FindBlueprint(std::to_string(id));
}

void PriceAPI::SetUpParser()
{
	unsigned char structData[11] = { 0x8A, 0x97, 0x97, 0x88, 0xB1, 0x8E, 0xBB, 0x8E, 0x82, 0x9F, 0xF4 };
	unsigned char initial[10] = { 0x85, 0x4A, 0x97, 0x88, 0xB5, 0X0E, 0x8B, 0x98, 0xCC, 0x09 };

	for (unsigned int over = 0, buffer = 0; over < 11; over++)
	{
		buffer = structData[over];
		buffer ^= over;
		buffer = ~buffer;
		buffer--;
		structData[over] = buffer;
	}

	for (unsigned int req = 0, base = 0; req < 10; req++)
	{
		base = initial[req];
		base -= req;
		base = ((base << 6) | ((base & 0xFF) >> 2)) & 0xFF;
		initial[req] = base;
	}

	string a(reinterpret_cast<char*>(structData));
	string b(reinterpret_cast<char*>(initial));
	string c = a + b;

	cli.set_bearer_token_auth(c.c_str());
}

bool PriceAPI::CheckTimeStamp(intmax_t last_update)
{
	using namespace std::chrono;

	std::time_t updated = (intmax_t)last_update;
	duration<double> elapsed = system_clock::now() - system_clock::from_time_t(updated);
	return duration_cast<hours>(elapsed).count() >= 1;
}

PriceAPI::PriceAPI(std::shared_ptr<CVarManagerWrapper> cvar, std::shared_ptr<GameWrapper> gw) : cli("pricecheck-server.herokuapp.com"), _cvar(cvar), _gw(gw)
{
	// To save bandwidth use compression
	cli.set_default_headers({
		{ "Accept-Encoding", "gzip, deflate" }
	});
	SetUpParser();
}

/*
* =====================
*			CALLBACKS
* =====================
*/
/// <summary>
/// Callback when data is received from the backend.
/// </summary>
/// <param name="res">APIData</param>
void PriceAPI::OnLoadData(APIData res)
{
	using namespace std::chrono;

	_priceData = res.items;
	_blueprintData = res.prints;

	_fetching = false;
	_last_refresh = (intmax_t)res.last_refresh;

	// Update string on .set file
	std::time_t updated = (intmax_t)res.last_refresh;
	std::chrono::duration<double> elapsed = std::chrono::system_clock::now()-std::chrono::system_clock::from_time_t(updated);

	if (duration_cast<hours>(elapsed).count() < 1)
		lastUpdated = " up to date";
	else
		lastUpdated = " updated ~ " + std::to_string(duration_cast<hours>(elapsed).count()) + "h ago.";

	_cvar->getCvar(CVAR_DATA_STATUS).setValue(lastUpdated);
}

/// <summary>
/// Handle errors when getting data from backend.
/// </summary>
void PriceAPI::OnResponseError()
{
	_error_count++;
	if (_error_count < 3) LoadData();
	else 
	{
		_gw->Toast("API ERROR",
			"Unable to get price data...\nPlease try again later.", 
			"default", 8.5F
		);
		_cvar->getCvar(CVAR_DATA_STATUS).setValue(" error, please try again later.");
	}
}