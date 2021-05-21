#pragma once
#include <bakkesmod/wrappers/canvaswrapper.h>
#include <bakkesmod/wrappers/items/TradeWrapper.h>
#include "gui/Fonts.h"

class PlayerTrade
{
	struct Value
	{
		int min = 0;
		int max = 0;
	};
	enum Currencies
	{
		Credits = 13
	};

public:
	PlayerTrade();	// Constructor
	~PlayerTrade();	// Destructor

	bool useAvg = false;

	void CheckTrade(TradeWrapper trade);
	void Clear();

	void Render(Fonts fonts, bool show);

private:
	Value sendValue = Value();
	Value recvValue = Value();

	void AddItems(ArrayWrapper<OnlineProductWrapper> items, Value& trader);
	void AddCredits(const std::vector<TradeWrapper::Currency>& currency, Value& trader);
	void CalculateColumns();
	ImColor colorProfit = { 126, 255, 187, 255 };
	ImColor colorLoss = { 255, 126, 126, 255 };

	/* ImGui specific */
	string menuName = "TradeWindow";
	float renderWidth = 190.f;
	float padding = 8.f;
	/* Maybe not the best practice, creating vector size of 4 to save column widths */
	std::vector<float> renderColums = std::vector<float>{ 0.f, 0.f, 32.f, 16.f };  //{}; //(4, 0.f);
	ImFont* fontTitle = nullptr;
	ImFont* fontText = nullptr;
};