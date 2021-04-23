#pragma once
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using string = std::string;
using json = nlohmann::json;

struct PaintPrice
{
  int min = 0;
  int max = 0;
};

struct Item
{
  string id;
  bool isError;
  intmax_t last_refresh;
  std::map<string, PaintPrice> data;
};

struct Blueprint
{
  string id;
  bool isError;
  intmax_t last_refresh;
  std::map<string, PaintPrice> data;
};

struct APIData
{
  bool isError;
  int count;
  intmax_t last_refresh;
  std::map<string, Item> items;
  std::map<string, Blueprint> prints;
};

struct APIResponse
{
  int count;
  intmax_t last_refresh;
  bool isError;
  std::map<string, Item> data;
};

void from_json(const json j, PaintPrice& p);
void from_json(const json j, Item& i);
void from_json(const json j, Blueprint& b);
void from_json(const json j, APIData& d);
void from_json(const json j, APIResponse& r);