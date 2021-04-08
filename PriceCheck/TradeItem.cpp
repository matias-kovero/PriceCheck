#include "pch.h"
#include "TradeItem.h"
#include "PriceCheck.h";


PaintPrice TradeItem::GetPrice(std::shared_ptr<GameWrapper> gw, std::shared_ptr<PriceAPI> api)
{
  priceInfo = Info();
  SpecialEditionDatabaseWrapper SE_DB = gw->GetItemsWrapper().GetSpecialEditionDB();
  priceInfo = this->updateItemInfo(SE_DB);
  string id = std::to_string(this->GetProductID());

  // Todo: move the location of this.
  Item price = getItem(id, api);
  std::string paintName = "default";
  if (!priceInfo.paint.empty())
  {
    paintName = priceInfo.paint;
    for (auto& c : paintName) c = tolower(c);
  }
  paint = paintName;
  auto p = price.data[paintName];
  
  return p;
}

Info TradeItem::updateItemInfo(SpecialEditionDatabaseWrapper sb)
{
  for (int i = 0; i < attributes.Count(); i++)
  {
    if (attributes.Get(i).GetAttributeType() == "ProductAttribute_TitleID_TA")
    {
      // Not implemented. Skip for now.
    }
    if (attributes.Get(i).GetAttributeType() == "ProductAttribute_Painted_TA") // Painted
    {
      auto pa = ProductAttribute_PaintedWrapper(attributes.Get(i).memory_address);
      auto color = pa.GetSortLabel().ToString();
      color.replace(0, 7, ""); // Removing "Painted" from paint color.
      priceInfo.paint = color;
    }
    if (attributes.Get(i).GetAttributeType() == "ProductAttribute_Certified_TA") // Certified
    {
      auto pa = ProductAttribute_CertifiedWrapper(attributes.Get(i).memory_address);
      auto cert = pa.GetRankLabel().ToString();
      priceInfo.certified = cert;
    }
    if (attributes.Get(i).GetAttributeType() == "ProductAttribute_SpecialEdition_TA") // Certified
    {
      auto pa = ProductAttribute_SpecialEditionWrapper(attributes.Get(i).memory_address);
      auto label = sb.GetSpecialEditionName(pa.GetEditionID());
      label.replace(0, 8, ""); // Removing "Edition_" from label.
      priceInfo.specialEdition = label;
    }
  }
  return priceInfo;
}

Item TradeItem::getItem(std::string id, std::shared_ptr<PriceAPI> api)
{
  auto data = api->priceData;
  auto it = data.find(id);
  if (it != data.end())
  {
    return it->second;
  }
  return api->FindItem(id);
}
