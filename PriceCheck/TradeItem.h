#pragma once
#include "bakkesmod/wrappers/items/OnlineProductWrapper.h"
#include "PriceAPI.h"

struct Info
{
  string title;
  string paint;
  string certified;
  string specialEdition;
  int paintID = 0;				// Index of paint string on paintMap
  int editionID = 0;			// unused
  unsigned char quality;	// unused
  int originalID = 0;			// This is the item id of the item on the blueprint.
};

class TradeItem : public OnlineProductWrapper
{
public:
  TradeItem(const OnlineProductWrapper& i) : OnlineProductWrapper(i) {}

  PaintPrice GetPrice();

  std::string paint;

private:
  Info itemInfo = Info();

  Info updateItemInfo();

	std::map<int, string> paintMap = {
		{0, "default"},
		{1, "crimson"},
		{2, "lime"},
		{3, "black"},
		{4, "sky blue"},
		{5, "cobalt"},
		{6, "burnt sienna"},
		{7, "forest green"},
		{8, "purple"},
		{9, "pink"},
		{10, "orange"},
		{11, "grey"},
		{12, "titanium white"},
		{13, "saffron"},
	};
};