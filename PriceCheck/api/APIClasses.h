#pragma once
#include "nlohmann/json.hpp"

using string = std::string;
using json = nlohmann::json;

enum class ITEMPAINT
{
  DEFAULT = 0,
  CRIMSON = 1,
  LIME = 2,
  BLACK = 3,
  SKYBLUE = 4,
  COBALT = 5,
  BURNTSIENNA = 6,
  FORESTGREEN = 7,
  PURPLE = 8,
  PINK = 9,
  ORANGE = 10,
  GREY = 11,
  TITANIUMWHITE = 12,
  SAFFRON = 13
};

struct COLOR
{
  int r = 0;
  int g = 0;
  int b = 0;
};

struct PaintPrice
{
  int min = 0;
  int max = 0;
};

struct Item
{
  string id;
  bool isError = false;
  intmax_t last_refresh = 0;
  std::map<ITEMPAINT, PaintPrice> data;
};

struct APIData
{
  bool isError;
  intmax_t last_refresh;
  std::map<string, Item> items;
  std::map<string, Item> prints;
};

string PaintToString(ITEMPAINT paint);
COLOR PaintToRGB(ITEMPAINT paint);

void from_json(const json j, PaintPrice& p);
void from_json(const json j, Item& i);
void from_json(const json j, APIData& d);