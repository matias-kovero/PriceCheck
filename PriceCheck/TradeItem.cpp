#include "pch.h"
#include "TradeItem.h"
#include "PriceCheck.h";


PaintPrice TradeItem::GetPrice()
{
  priceInfo = Info();
  priceInfo = this->updateItemInfo();
  Item price = getItem();

  // Update paint field, currently used only for logging.
  paint = _globalPriceAPI->paintNameList[priceInfo.paintID];

  auto p = price.data[paint];
  
  return p;
}
// TODO: add the ability to find paint price by paint id. (int)
Info TradeItem::updateItemInfo()
{
  for (int i = 0; i < attributes.Count(); i++)
  {
    if (attributes.Get(i).GetAttributeType() == "ProductAttribute_TitleID_TA")
    {
      // Not implemented. Skip for now.
    }
    if (attributes.Get(i).GetAttributeType() == "ProductAttribute_Quality_TA")
    {
      // Why all items don't have this?
      auto pa = ProductAttribute_QualityWrapper(attributes.Get(i).memory_address);
      priceInfo.quality = pa.GetQuality();
    }
    if (attributes.Get(i).GetAttributeType() == "ProductAttribute_Painted_TA") // Painted
    {
      auto pa = ProductAttribute_PaintedWrapper(attributes.Get(i).memory_address);
      priceInfo.paintID = pa.GetPaintID();
    }
    if (attributes.Get(i).GetAttributeType() == "ProductAttribute_Certified_TA") // Certified
    {
      auto pa = ProductAttribute_CertifiedWrapper(attributes.Get(i).memory_address);
      auto cert = pa.GetRankLabel().ToString();
      priceInfo.certified = cert;
    }
    if (attributes.Get(i).GetAttributeType() == "ProductAttribute_SpecialEdition_TA") // Is this Obsolete now?
    {
      auto pa = ProductAttribute_SpecialEditionWrapper(attributes.Get(i).memory_address);
      auto label = _globalSpecialEditionManager->GetSpecialEditionName(pa.GetEditionID());
      label.replace(0, 8, ""); // Removing "Edition_" from label.
      priceInfo.editionID = pa.GetEditionID();
      priceInfo.specialEdition = label;
    }
  }
  return priceInfo;
}

Item TradeItem::getItem()
{
  string id = std::to_string(this->GetProductID());
  auto data = _globalPriceAPI->priceData;
  auto it = data.find(id);
  if (it != data.end())
  {
    return it->second;
  }
  return _globalPriceAPI->FindItem(id);
}
