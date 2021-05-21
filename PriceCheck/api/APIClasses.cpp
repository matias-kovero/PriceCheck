#include "pch.h"
#include "APIClasses.h"

/*
* Many thanks to Martinn for guiding and helping 
* me with the Nlohmann JSON library
*/
#define J(var) j.at(#var).get_to(p.var);
#define J2(var, var2) j.at(#var).get_to(p.var2);
#define OBJ(var) j.get_to(p.var);

#define JOPT(var) if(j.find(#var) != j.end()) { j.at(#var).get_to(p.var); }
#define JOPT2(var, var2) if(j.find(#var) != j.end()) { j.at(#var).get_to(p.var2); }

ITEMPAINT ToItemPaint(const string& id)
{
  // Notes on unordered_map vs map:
  // For lookup tables/relative static maps use unordered -> faster
  // If need tons of insertions/deletions -> use map
  static std::unordered_map<string, ITEMPAINT> table
  {
    {"0", ITEMPAINT::DEFAULT},
    {"1", ITEMPAINT::CRIMSON},
    {"2", ITEMPAINT::LIME},
    {"3", ITEMPAINT::BLACK},
    {"4", ITEMPAINT::SKYBLUE},
    {"5", ITEMPAINT::COBALT},
    {"6", ITEMPAINT::BURNTSIENNA},
    {"7", ITEMPAINT::FORESTGREEN},
    {"8", ITEMPAINT::PURPLE},
    {"9", ITEMPAINT::PINK},
    {"a", ITEMPAINT::ORANGE},
    {"b", ITEMPAINT::GREY},
    {"c", ITEMPAINT::TITANIUMWHITE},
    {"d", ITEMPAINT::SAFFRON},
  };
  if (const auto it = table.find(id); it != table.end())
  {
    return it->second;
  }
  throw std::invalid_argument("Argument to paint mapping is unknown");
}

string PaintToString(ITEMPAINT paint)
{
  switch (paint) 
  {
  case ITEMPAINT::DEFAULT:
    return "";
  case ITEMPAINT::CRIMSON:
    return "Crimson";
  case ITEMPAINT::LIME:
    return "Lime";
  case ITEMPAINT::BLACK:
    return "Black";
  case ITEMPAINT::SKYBLUE:
    return "Sky Blue";
  case ITEMPAINT::COBALT:
    return "Cobalt";
  case ITEMPAINT::BURNTSIENNA:
    return "Burnt Sienna";
  case ITEMPAINT::FORESTGREEN:
    return "Forest Green";
  case ITEMPAINT::PURPLE:
    return "Purple";
  case ITEMPAINT::PINK:
    return "Pink";
  case ITEMPAINT::ORANGE:
    return "Orange";
  case ITEMPAINT::GREY:
    return "Grey";
  case ITEMPAINT::TITANIUMWHITE:
    return "Titanium White";
  case ITEMPAINT::SAFFRON:
    return "Saffron";
  }
  return "???";
}

COLOR PaintToRGB(ITEMPAINT paint)
{
  switch (paint)
  {
  case ITEMPAINT::DEFAULT:
    return {0,0,0};
  case ITEMPAINT::CRIMSON:
    return { 164,22,22 };
  case ITEMPAINT::LIME:
    return { 144,255,17 };
  case ITEMPAINT::BLACK:
    return { 0,0,0 };
  case ITEMPAINT::SKYBLUE:
    return { 22,142,221 };
  case ITEMPAINT::COBALT:
    return { 24,54,255 };
  case ITEMPAINT::BURNTSIENNA:
    return { 93,32,17 };
  case ITEMPAINT::FORESTGREEN:
    return { 25,143,25 };
  case ITEMPAINT::PURPLE:
    return { 87,20,151 };
  case ITEMPAINT::PINK:
    return { 213,60,162 };
  case ITEMPAINT::ORANGE:
    return { 255,85,9 };
  case ITEMPAINT::GREY:
    return { 72,72,72 };
  case ITEMPAINT::TITANIUMWHITE:
    return { 255,255,255 };
  case ITEMPAINT::SAFFRON:
    return { 215,211,24 };
  }
  return COLOR();
}

COLOR QualityToRGB(int quality)
{
  switch (quality)
  {
  case 0:
    return { 0,0,0 }; // 
  case 1:
    return { 54, 85, 99 }; // Uncommon
  case 2:
    return { 59, 77, 116 }; // Rare
  case 3:
    return { 56, 49, 88 }; // Very Rare
  case 4:
    return { 104, 27, 27 }; // Import
  case 5:
    return { 181, 165, 64 }; // Exotic
  case 6:
    return { 143, 26, 191 }; // Black Market
  case 7:
    return { 25,143,25 }; // ??
  case 8:
    return { 125, 72, 49 }; // Limited
  }
  return COLOR();
}

void from_json(const json j, PaintPrice& p)
{
  try 
  {
    auto prices = j.get<std::vector<int>>();
    if (prices.size() != 2)
    {
      LOG("PaintPrice seems invalid, should have 2 values");
      return;
    }
    p.min = prices[0];
    p.max = prices[1];
  }
  catch (std::exception& e)
  {
    LOG("Exeption in {}[line{}]: {}", __FUNCTION__, __LINE__, e.what());
  }
}

void from_json(const json j, Item& p)
{
  try 
  {
    auto tmp_data = j.get<std::map<string, PaintPrice>>();
    for (auto& [id, prices] : tmp_data)
    {
      auto item_paint = ToItemPaint(id);
      p.id = id; // For some reason this is not set.
      p.data[item_paint] = prices;
    }
  }
  catch (std::exception& e)
  {
    LOG("Exeption in {}[line{}]: {}", __FUNCTION__, __LINE__, e.what());
  }
}

void from_json(const json j, APIData& p)
{
  J2(error, isError);
  J2(time, last_refresh);
  J(items);
  J(prints);

  for (auto& [id, item] : p.items)
  {
    item.id = id;
    item.isError = p.isError;
    item.last_refresh = p.last_refresh;
  }

  for (auto& [id, bp] : p.prints)
  {
    bp.id = id;
    bp.isError = p.isError;
    bp.last_refresh = p.last_refresh;
  }
}

#undef J
#undef J2
#undef JOPT
#undef JOPT2
#undef OBJ