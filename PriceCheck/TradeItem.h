#pragma once
#include "bakkesmod/wrappers/items/OnlineProductWrapper.h"

struct Info
{
	int id = 0;
	ITEMPAINT paint = ITEMPAINT::DEFAULT;
};

class TradeItem : public OnlineProductWrapper
{
public:
  TradeItem(const OnlineProductWrapper& i) : OnlineProductWrapper(i) {}

  PaintPrice GetPrice();
	string GetPaint();

private:
  Info info = Info();
  Info updateItemInfo();
};