#pragma once

#include "bakkesmod/wrappers/items/OnlineProductWrapper.h"
#include "PriceAPI.h"

class TradeItem : public OnlineProductWrapper
{
public:
  TradeItem(const OnlineProductWrapper& i) : OnlineProductWrapper(i) {}

  PaintPrice GetPrice(std::shared_ptr<GameWrapper> gw, std::shared_ptr<PriceAPI> api);

  std::string paint;

private:
  Info priceInfo;

  ArrayWrapper<ProductAttributeWrapper> attributes = this->GetAttributes();

  Info updateItemInfo(SpecialEditionDatabaseWrapper sb);

  Item getItem(std::string id, std::shared_ptr<PriceAPI> api);
};