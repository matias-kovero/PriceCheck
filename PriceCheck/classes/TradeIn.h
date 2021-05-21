#pragma once

#include "TradeItem.h"
#include "classes/ItemSeries.h"
#include "gui/Fonts.h" // Why it isn't enough to include this in TradeIn.cpp???

class TradeIn
{
	struct Value
	{
		int min = 0;
		int max = 0;
	};

	struct Item
	{
		int min = 0;
		int max = 0;
		int series = 0;
		string label = "";
		string paintLabel = "";
		COLOR paint = COLOR();
	};

	struct Series
	{
		int id = 0;
		int itemCount = 0;
		ItemSeriesDatabaseWrapper::Series data;

		bool operator<(const Series& a) const { return itemCount > a.itemCount; }
	};

public:
	TradeIn();	// Constructor
	~TradeIn();	// Destructor

	bool useAvg = false;

	void AddItem(TradeItem item);
	void Clear();

	void Render(Fonts fonts, bool show);

private:
	int quality = 0;
	Value value = Value();
	ItemSeriesDatabaseWrapper itemSeriesDatabase = ItemSeriesDatabaseWrapper();

	std::vector<Item> items;
	std::vector<Series> series;

	std::vector<Series> UpdateSeries(const int& id);

	/* Section specific renders */
	void RenderItems();
	void RenderOutcome();

	// On a item change, calculate column widths and cache result
	void CalculateRenderInfo();

	/* Cached render info to reduce calculations */
	string menuName = "TradeInWindow";
	float renderWidth = 250.f;
	float additionalHeight = 0.f;
	/* Maybe not the best practice, creating vector size of 5 to save column widths */
	std::vector<float> renderColums = std::vector<float>(5, 0.f);
	/* Cache fonts, no need to fetch the everytime */
	ImFont* fontTitle = nullptr;
	ImFont* fontText = nullptr;
};