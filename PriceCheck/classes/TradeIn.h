#pragma once
#include "TradeItem.h"

struct Format
{
	float name = 0, paint = 0, value = 0, width = 0, min = 0, max = 0;
};

struct TradeInItem
{
	int min = 0;
	int max = 0;
	string name = "";
	string paint = "";
	COLOR paintcolor = COLOR();
	string series = "";
	/* Series ID */
	int id = 0;
};

struct TradeInValue
{
	int min = 0;
	int max = 0;
};

class TradeIn
{
public:
	TradeIn();	// Constructor
	~TradeIn();	// Destructor

	void AddItem(TradeItem item);
	void Clear();

	void Render(CanvasWrapper canvas, Vector2 screenSize, Vector2F pos, bool useAvg);
	Format CalculateLongestString(CanvasWrapper canvas, int padding);
private:
	std::vector<TradeInItem> items;
	TradeInValue value = TradeInValue();
	std::map<string, int> calculateChances();
};