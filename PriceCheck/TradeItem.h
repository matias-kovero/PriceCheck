#pragma once

#include "bakkesmod/wrappers/items/OnlineProductWrapper.h"
#include "PriceAPI.h"

class TradeItem : public OnlineProductWrapper
{
public:
  TradeItem(const OnlineProductWrapper& i) : OnlineProductWrapper(i) {}

  PaintPrice GetPrice();

  std::string paint;

private:
  Info itemInfo = Info();

  ArrayWrapper<ProductAttributeWrapper> attributes = this->GetAttributes();

  Info updateItemInfo();

  Item getItem();
};