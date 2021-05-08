#include "pch.h"
#include "TradeIn.h"

TradeIn::TradeIn()
{
}

TradeIn::~TradeIn()
{
	// If we have dynamically allocated memory "new" or pointers delete them here.
	// This must be done to avoid memory leaks.
}

void TradeIn::AddItem(TradeItem item)
{
	PaintPrice price = item.GetPrice();
	value.min += price.min;
	value.max += price.max;
	items.push_back({
		price.min,
		price.max,
		item.GetLongLabel().ToString(),
		item.GetPaint(),
		item.GetPaintColor(),
		item.GetSeries()
	});
}

void TradeIn::Clear()
{
	value = TradeInValue();
  items.clear();
}

void TradeIn::Render(CanvasWrapper canvas, Vector2 screenSize, Vector2F pos, bool useAvg)
{
	int box_padding = 5; // Paddings around the box sides.
	int category_padding = 5; // Padding between displayed info (name, paint, value)
	Format rf = CalculateLongestString(canvas, box_padding);

	int box_width = rf.width > 250 ? rf.width : 250, box_height = 50 + (items.size() * 15 * 2) + (box_padding * 2);
	// Position box to screen.
	Vector2F normPos = {
		(pos.X + 1.0) * (screenSize.X * 0.5 - box_width / 2),
		(-pos.Y + 1.0) * (screenSize.Y * 0.5 - box_height / 2)
	};
	Vector2 givePos = { (int)normPos.X, (int)normPos.Y };

	// Where second half Y starts
	int sHalf = givePos.Y + 40 + (int)(items.size() * 15);

	// Draw Box - give the ability to change alpha?
	canvas.SetColor(6, 23, 35, 230);
	canvas.SetPosition(givePos);
	canvas.FillBox(Vector2{ box_width, box_height });

	string tMin = std::to_string(value.min);
	string tMax = std::to_string(value.max);

	int tSum = (value.min + value.max) / 2;

	canvas.SetColor(255, 255, 255, 255);
	canvas.SetPosition(Vector2{ givePos.X + 5, givePos.Y + 5 });

	if (useAvg)
		canvas.DrawString("Trade In Value: " + std::to_string(tSum));
	else
		canvas.DrawString("Trade In Value: " + tMin + " - " + tMax);
	int i = 1;
	// Love the range-based loop, but why no index :(
	for (const auto& item : items)
	{
		i++;
		//canvas.SetColor(255, 255, 255, 255);
		//canvas.SetPosition(Vector2{ givePos.X + 5, givePos.Y + (15*i) });
		// Draw name
		canvas.SetColor(113, 148, 242, 255);
		canvas.SetPosition(Vector2{ givePos.X + box_padding, givePos.Y + (i * 15) });
		canvas.DrawString(item.name);
		/* DRAW PAINT */
		// Background
		if (item.paint != "")
		{
			canvas.SetColor(item.paintcolor.r, item.paintcolor.g, item.paintcolor.b, 255);
			canvas.SetPosition(Vector2{
				(givePos.X + box_padding) + (int)rf.name,
				givePos.Y + (i * 15) + 1
				}
			);
			canvas.FillBox(Vector2{ (int)rf.paint, 13 });
		}
		// Name
		canvas.SetColor(item.paintcolor.r, item.paintcolor.g, item.paintcolor.b, 255);
		canvas.SetPosition(Vector2{ (givePos.X + box_padding) + (int)rf.name, givePos.Y + (i * 15) });
		//canvas.DrawString(item.paint);
		/* DRAW PRICE */
		// Max
		canvas.SetColor(255, 255, 255, 255);
		canvas.SetPosition(Vector2{
			(givePos.X + box_width) - box_padding - (int)canvas.GetStringSize(std::to_string(item.max)).X,
			givePos.Y + i * 15
			});
		canvas.DrawString(std::to_string(item.max));
		// Min
		canvas.SetColor(255, 255, 255, 255);
		canvas.SetPosition(Vector2{
			(givePos.X + box_width) - box_padding - (int)rf.max - (int)canvas.GetStringSize(std::to_string(item.min) + " -").X,
			givePos.Y + i * 15
			});
		canvas.DrawString(std::to_string(item.min) + " -");
	}
	// Divider line
	canvas.SetColor(255, 255, 255, 130);
	canvas.DrawLine(
		Vector2{ givePos.X + box_padding, sHalf - 5 },
		Vector2{ givePos.X + box_width - box_padding, sHalf - 5 }
	);
	// Outcome
	canvas.SetColor(255, 255, 255, 255);
	canvas.SetPosition(Vector2{ givePos.X + box_padding, sHalf });
	canvas.DrawString("Outcome:");
	int s = 1;
	// Calculate the chances here?
	auto tally = calculateChances();
	for (auto const& [key, val] : tally)
	{
		s++;
		int chance = (double)tally[key] / items.size() * 100; // Avoid integer math

		canvas.SetColor(113, 148, 242, 255);
		canvas.SetPosition(Vector2{ givePos.X + box_padding, sHalf + (s * 15) });
		canvas.DrawString("(" +
			// Calculate %
			std::to_string(chance)
			+ "%) " + key);
	}
}

Format TradeIn::CalculateLongestString(CanvasWrapper canvas, int padding)
{
	Format rf;
	// Not the best logic, might need optimization - as this is calculated every tick!
	for (const auto& item : items)
	{
		// canvas.GetStringSize();
		float i_name = canvas.GetStringSize(item.name).X,
			i_paint = canvas.GetStringSize(item.paint).X,
			i_value = canvas.GetStringSize(std::to_string(item.min) + " - " + std::to_string(item.max)).X,
			i_min = canvas.GetStringSize(std::to_string(item.min)).X,
			i_max = canvas.GetStringSize(std::to_string(item.max)).X;

		rf.name = i_name > rf.name ? i_name : rf.name;
		rf.paint = i_paint > rf.paint ? i_paint : rf.paint;
		rf.value = i_value > rf.value ? i_value : rf.value;
		rf.min = i_min > rf.min ? i_min : rf.min;
		rf.max = i_max > rf.max ? i_max : rf.max;
	}
	rf.paint = 15; // Forced width of paint rect
	rf.name += padding, rf.paint += padding, rf.value += padding, rf.min += padding, rf.max += padding;
	rf.width = rf.name + rf.paint + rf.value + (padding * 5); // Left + Right pad

	//LOG("{} | {} | {} = {}", rf.name, rf.paint, rf.value, rf.width);
	return rf;
}

std::map<string, int> TradeIn::calculateChances()
{
	// O(n lg n) => possible O(n) with unordered_map
	std::map<string, int> count;
	for (auto x : items)
		++count[x.series];
	// count.size() has the unique
	// count["serie"] has the freq
	return count;
}
