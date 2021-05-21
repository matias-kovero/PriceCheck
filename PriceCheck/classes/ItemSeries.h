#pragma once

class ItemSeriesDatabaseWrapper
{
public:

  struct SeriesItem
  {
    string label = "";
    int quality = 0;
    std::map<ITEMPAINT, PaintPrice> price;
  };

  struct Series
  {
    int id = 0;
    string name = "";
    // Should I use namespace? -> to avoid using <class::struct> and only use <struct>
    std::vector<ItemSeriesDatabaseWrapper::SeriesItem> items;
  };

  // Get series name
  string ToSeriesString(const int& id);
  // Vector of item ids in given series
  std::vector<int> SeriesToItems(const int& id);
  // Helper to display item quality - should move under items?
  string QualityToString(const int& quality);

  Series GetSeriesItems(const int& id);

private:
  // When series items are requested, cache the results here?
  std::map<int, Series> cache;
  // Get item price info from the api.
  Series FillSeries(const int& id);
};