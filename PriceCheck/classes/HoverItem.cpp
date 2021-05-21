#include "pch.h"
#include "HoverItem.h"
#include "TradeItem.h"

HoverItem::HoverItem()
{
}

HoverItem::~HoverItem()
{
}

void HoverItem::OnHover()
{
  hover = true;
  m_item = CachedItem();
}

void HoverItem::Clear()
{
  hover = false;
  m_item = CachedItem();
  lastHover = 0;
}

void HoverItem::HandleItem(OnlineProductWrapper item)
{
  if (item.IsNull()) return;
  auto id = item.GetInstanceID();
  
  if (id != lastHover)
  {
    lastHover = id;
    TradeItem i = item;
    if (!i)
    {
      LOG("{}: Item is null", __FUNCTION__);
      return;
    }
    LOG("{}: IsBlueprint={}",item.GetLongLabel().ToString(), item.IsBlueprint());
    PaintPrice price = i.GetPrice();
    string paint = i.GetPaint();

    m_item = {
      i.GetLongLabel().ToString(),
      paint,
      i.GetPaintColor(),
      i.GetQualityColor(),
      price.min,
      price.max
    };

    //LOG("{}: {} ({} - {})", __FUNCTION__, i.GetLongLabel().ToString(), price.min, price.max);
  }
}
/* Should move to use ImGui Render
void HoverItem::Render(CanvasWrapper canvas, Vector2 pos, Vector2F size, bool useAvg)
{
  if (m_item.name.empty())
  {
    // LOG("Item name is empty: {}", m_item.name);
    return;
  }
  bool hasPaint = m_item.paint != "";
  // Create class for canvas formatting
  // Should give positions for different sections
  // Overall element, width + height

  int box_padding = 7; // Paddings around the box sides.
  int category_padding = 5; // Padding between displayed info (name, paint, value)

  FormatHoverItem hi = CanvasTools::HoverItem(canvas, m_item);
  int box_width = hi.width > 200 ? hi.width : 200, box_height = (15 * (hasPaint ? 3 : 2)) + (box_padding * 2);
  // Position box to screen.
  Vector2F normPos = {
    (size.X + 1.0) * (pos.X * 0.5 - box_width / 2),
    (-size.Y + 1.0) * (pos.Y * 0.5 - box_height / 2)
  };

  Vector2 givePos = { (int)normPos.X, (int)normPos.Y };

  CanvasTools::Underglow(canvas, givePos, Vector2{ box_width, box_height });

  // Draw Box - give the ability to change alpha?
  // Remove rariry colors, use default colors from game
  //canvas.SetColor(m_item.qualitycolor.r, m_item.qualitycolor.g, m_item.qualitycolor.b, 210);
  canvas.SetColor(41, 69, 119, 255);
  canvas.SetPosition(givePos);
  canvas.FillBox(Vector2{ box_width, box_height });

  CanvasTools::Borders(canvas, givePos, Vector2{ box_width, box_height });

  string tMin = std::to_string(m_item.min);
  string tMax = std::to_string(m_item.max);

  int tSum = (m_item.min + m_item.max) / 2;
  // Item Name
  canvas.SetColor(255, 255, 255, 255);
  canvas.SetPosition(Vector2{ givePos.X + box_padding, givePos.Y + box_padding });
  canvas.DrawString(m_item.name);

  if (hasPaint)
  {
    canvas.SetColor(m_item.paintcolor.r, m_item.paintcolor.g, m_item.paintcolor.b, 255);
    canvas.SetPosition(Vector2{ givePos.X + box_padding, givePos.Y + 15 + box_padding });
    canvas.DrawString(m_item.paint);
  }

  canvas.SetColor(255, 255, 255, 255);
  canvas.SetPosition(Vector2{ givePos.X + box_padding, givePos.Y + (hasPaint ? 15 * 2 : 15) + box_padding });

  if (useAvg)
    canvas.DrawString(std::to_string(tSum));
  else
    canvas.DrawString(tMin + " - " + tMax);
}
*/