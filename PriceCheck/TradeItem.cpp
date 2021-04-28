#include "pch.h"
#include "TradeItem.h"


PaintPrice TradeItem::GetPrice()
{
  // Check item attributes
  info = this->updateItemInfo();

  Item price = this->IsBlueprint() ?
    _globalPriceAPI->FindBlueprint(info.id) :
    _globalPriceAPI->FindItem(this->GetProductID());

  return price.data[info.paint];

}

string TradeItem::GetPaint()
{
  // Is this needed, I belive need to double check Info has been filled with data.
  // Could be done when this Class gets created?
  info = this->updateItemInfo();

  return PaintToString(info.paint);
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
      info.id = pa.GetProductID(); // Get the manufactured products id. Used to get price info.
    }
    if (att.Get(i).GetAttributeType() == "ProductAttribute_Quality_TA")
    {
      // No usage yet
      /*
      auto pa = ProductAttribute_QualityWrapper(att.Get(i).memory_address);
      info.quality = pa.GetQuality();
      */
    }
    if (att.Get(i).GetAttributeType() == "ProductAttribute_Painted_TA") // Painted
    {
      auto pa = ProductAttribute_PaintedWrapper(att.Get(i).memory_address);
      info.paint = static_cast<ITEMPAINT>(pa.GetPaintID());
    }
    if (att.Get(i).GetAttributeType() == "ProductAttribute_Certified_TA") // Certified
    {
      // No usage yet
      /*
      auto pa = ProductAttribute_CertifiedWrapper(att.Get(i).memory_address);
      info.certified = pa.GetRankLabel().ToString();
      */
    }
    if (att.Get(i).GetAttributeType() == "ProductAttribute_SpecialEdition_TA") // Special Editions
    {
      // No usage yet
      /*
      auto pa = ProductAttribute_SpecialEditionWrapper(att.Get(i).memory_address);
      auto label = _globalSpecialEditionManager->GetSpecialEditionName(pa.GetEditionID());
      label.replace(0, 8, ""); // Removing "Edition_" from label.
      info.specialEdition = label;
      */
    }
  }
  return info;
}