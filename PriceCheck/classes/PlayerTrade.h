#pragma once
#include <bakkesmod/wrappers/canvaswrapper.h>
#include <bakkesmod/wrappers/items/TradeWrapper.h>

struct TradeValue
{
	int min = 0;
	int max = 0;
};

enum Currencies
{
	Credits = 13
};

class PlayerTrade
{
public:
	PlayerTrade();	// Constructor
	~PlayerTrade();	// Destructor

	void CheckTrade(TradeWrapper trade);
	void Clear();

	void Render(CanvasWrapper canvas, Vector2 pos, Vector2 size, bool useAvg);

private:
	TradeValue sendValue = TradeValue();
	TradeValue recvValue = TradeValue();

	void AddItems(ArrayWrapper<OnlineProductWrapper> items, TradeValue& trader);
	void AddCredits(std::vector<TradeWrapper::Currency> currency, TradeValue& trader);
};