#pragma once

struct CachedItem
{
  string name;
  string paint;
  COLOR paintcolor;
  COLOR qualitycolor;
  int min;
  int max;
};

class HoverItem
{
public:
  HoverItem();	// Constructor
  ~HoverItem();	// Destructor

  void OnHover();
  void Clear();
  void HandleItem(OnlineProductWrapper item);

  // Should move to ImGui Render
  //void Render(CanvasWrapper canvas, Vector2 pos, Vector2F size, bool useAvg);

  bool hover = false;

private:

  bool isSame = false;
  unsigned long long lastHover = 0;
  CachedItem m_item = CachedItem();
};