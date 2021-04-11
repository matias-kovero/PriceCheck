#include "pch.h"
#include "TradeItem.h"
#include "PriceCheck.h";


PaintPrice TradeItem::GetPrice()
{
  itemInfo = this->updateItemInfo();
  Item price = _globalPriceAPI->FindItem(this->GetProductID());

  // Update paint field, currently used only for logging.
  paint = _globalPriceAPI->paintNameList[itemInfo.paintID];

  auto p = price.data[paint];
  
  return p;
}
// TODO: add the ability to find paint price by paint id. (int)
Info TradeItem::updateItemInfo()
{
  auto att = this->GetAttributes();
  for (int i = 0; i < att.Count(); i++)
  {
    if (att.Get(i).GetAttributeType() == "ProductAttribute_TitleID_TA")
    {
      // Not implemented. Skip for now.
    }
    if (att.Get(i).GetAttributeType() == "ProductAttribute_Quality_TA")
    {
      // Why all items don't have this?
      auto pa = ProductAttribute_QualityWrapper(att.Get(i).memory_address);
      itemInfo.quality = pa.GetQuality();
    }
    if (att.Get(i).GetAttributeType() == "ProductAttribute_Painted_TA") // Painted
    {
      auto pa = ProductAttribute_PaintedWrapper(att.Get(i).memory_address);
      itemInfo.paintID = pa.GetPaintID();
    }
    if (att.Get(i).GetAttributeType() == "ProductAttribute_Certified_TA") // Certified
    {
      auto pa = ProductAttribute_CertifiedWrapper(att.Get(i).memory_address);
      auto cert = pa.GetRankLabel().ToString();
      itemInfo.certified = cert;
    }
    if (att.Get(i).GetAttributeType() == "ProductAttribute_SpecialEdition_TA") // Is this Obsolete now?
    {
      auto pa = ProductAttribute_SpecialEditionWrapper(att.Get(i).memory_address);
      auto label = _globalSpecialEditionManager->GetSpecialEditionName(pa.GetEditionID());
      label.replace(0, 8, ""); // Removing "Edition_" from label.
      itemInfo.editionID = pa.GetEditionID();
      itemInfo.specialEdition = label;
    }
  }
  return itemInfo;
}

/*
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
*/