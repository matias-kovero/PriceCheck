#include "pch.h"
#include "TradeItem.h"


PaintPrice TradeItem::GetPrice()
{
  Item price = this->IsBlueprint() ?
    _globalPriceAPI->FindBlueprint(m_info.id) :
    _globalPriceAPI->FindItem(this->GetProductID());

  return price.data[m_info.paint];
}

string TradeItem::GetPaint()
{
  return PaintToString(m_info.paint);
}

COLOR TradeItem::GetPaintColor()
{
  return PaintToRGB(m_info.paint);
}

string TradeItem::GetSeries()
{
  try 
  {
    int id = this->IsBlueprint() ?
      this->GetBlueprintSeriesID() :
      this->GetSeriesID();
    string name = ToSeriesString(id);
    return name;
  }
  catch (std::exception& e) 
  {
    LOG("Exception in {}: {}", __FUNCTION__, e.what());
    return "";
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
      m_info.id = pa.GetProductID(); // Get the manufactured products id. Used to get price info.
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
      m_info.paint = static_cast<ITEMPAINT>(pa.GetPaintID());
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
  return m_info;
}

string TradeItem::ToSeriesString(const int& id)
{
  static std::unordered_map<int, string> table
  {
    //{   1, ""}, // Normal items null series??
    {  10, "Turbo"},
    {  11, "Nitro"},
    {  13, "Overdrive"},
    {  18, "Accelerator"},
    {  26, "Victory"},
    {  29, "Triumph"},
    {  47, "Golden Egg '18"},
    {  48, "Zephyr"},
    { 190, "Elevation"},
    { 191, "Golden Pumpkin '18"},
    { 207, "Golden Gift '18"},
    { 299, "Ferocity"},
    { 300, "Golden Lantern '19"},
    { 317, "Impact"},
    { 403, "Totally Awesome"},
    { 443, "Golden Egg '19"},
    { 534, "Vindicator"},
    { 542, "Golden Gift '19"},
    { 636, "Revival"},
    { 737, "Ignition"},
    { 855, "Accolade"},
    { 900, "Momentum"},
    { 902, "Golden Egg '20"},
    {1032, "Season 1"},
    //{1147, ""}, // Torunament items "null" series ??
    {1154, "Golden Gift '20"},
    {1180, "Season 2"},
    {1181, "Golden Lantern '21"},
    {1204, "Accolade III"},
  };
  if (const auto it = table.find(id); it != table.end())
  {
    return it->second + " Series";
  }
  throw std::invalid_argument("Unkown series: " + std::to_string(id));
}