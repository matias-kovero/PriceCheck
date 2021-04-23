#include "pch.h"
#include "TradeItem.h"


PaintPrice TradeItem::GetPrice()
{
  // Check item attributes
  itemInfo = this->updateItemInfo();

  if (this->IsBlueprint()) 
  {
    Blueprint price = _globalPriceAPI->FindBlueprint(itemInfo.originalID);
    paint = paintMap[itemInfo.paintID];

    auto p = price.data[paint];
    return p;
  }
  else 
  {
    Item price = _globalPriceAPI->FindItem(this->GetProductID());
    paint = paintMap[itemInfo.paintID];

    auto p = price.data[paint];
    return p;
  }
}

Info TradeItem::updateItemInfo()
{
  auto att = this->GetAttributes();
  for (int i = 0; i < att.Count(); i++)
  {
    if (att.Get(i).GetAttributeType() == "ProductAttribute_TitleID_TA")
    {
      // Not implemented. Skip for now.
    }
    if (att.Get(i).GetAttributeType() == "ProductAttribute_Blueprint_TA")
    {
      auto pa = ProductAttribute_BlueprintWrapper(att.Get(i).memory_address);
      itemInfo.originalID = pa.GetProductID();
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
