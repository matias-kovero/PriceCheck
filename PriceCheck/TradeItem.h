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
  TradeItem(const OnlineProductWrapper& i) : OnlineProductWrapper(i) { m_info = updateItemInfo(); }

  PaintPrice GetPrice();
  string GetPaint();
  COLOR GetPaintColor();
  string GetSeries();

private:
  Info m_info = Info();
  Info updateItemInfo();
  string ToSeriesString(const int& id);
};