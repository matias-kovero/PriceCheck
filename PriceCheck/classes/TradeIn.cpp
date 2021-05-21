#include "pch.h"
#include "TradeIn.h"
#include "gui/GUITools.h"
#include "gui/Fonts.h"

TradeIn::TradeIn(){}

TradeIn::~TradeIn(){}

void TradeIn::AddItem(TradeItem item)
{
	// Set the quality of the trade in
	if (items.size() < 1) quality = item.GetQuality() + 1;

	PaintPrice price = item.GetPrice();
	value.min += price.min;
	value.max += price.max;

	int seriesId = item.IsBlueprint() ? item.GetBlueprintSeriesID() : item.GetSeriesID();
	/*
	* int min = 0;
	* int max = 0;
	* int series = 0;
	* string label = "";
	* string paintLabel = "";
	* COLOR paint = COLOR();
	*/
	items.push_back(
		{
			price.min,
			price.max,
			seriesId,
			item.GetLongLabel().ToString(),
			item.GetPaint(),
			item.GetPaintColor()
		}
	);

	// Update our series info to show outcome
	series = UpdateSeries(seriesId);

	// Update our render cache
	CalculateRenderInfo();
}

void TradeIn::Clear()
{
	/* Reset values to default */
	value = Value();
  items.clear();
	series.clear();
	quality = 0;
	/* Set render stuff to default */
	renderWidth = 250.f;
	additionalHeight = 0.f;
}

void TradeIn::Render(Fonts fonts, bool show)
{
	ImVec2 box_size = { renderWidth, 100.f + additionalHeight + (items.size() + series.size()) * ImGui::GetTextLineHeight() * 1.25f + 16.f }; // This should change with code?

	ImGui::SetNextWindowSize(box_size); // As we dynamically change the size, need to run every frame

	// Yeet out for optimization
	if (!ImGui::Begin(menuName.c_str(), &show,
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize )) // | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs))
	{
		// Early out
		return;
	}

	float padding = 8.f;
	// Check fonts, if not cached - cache them.
	if (!fontTitle) fontTitle = fonts.GetFont("RLHeadI");
	if (!fontText) fontText = fonts.GetFont("default");

	// Start rendering content
	GUITools::BoxShadow(ImGui::GetWindowPos(), ImGui::GetWindowSize(), 1.1f, true);

	// Header
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { padding, padding });
	ImGui::BeginChildFrame(10001, { 0.f, 35.f }, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
	ImGui::PushFont(fontTitle);
	ImGui::Text("TRADE IN");
	GUITools::AlignRight(std::to_string(value.min) + " - " + std::to_string(value.max), ImGui::GetWindowContentRegionMax().x);
	ImGui::PopFont();
	ImGui::EndChildFrame();

	// TradeIn Items
	fontText->Scale = 1.1f;
	ImGui::PushFont(fontText);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(80, 80, 80, 40));
	ImGui::BeginChild("ItemsBG", { 0, (items.size()) * ImGui::GetTextLineHeight() * 1.25f + padding * 2 });
	ImGui::PopStyleColor();
	ImGui::BeginChildFrame(100002, { 0, (items.size()) * ImGui::GetTextLineHeight() * 1.25f + padding * 2 }, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
	RenderItems();
	ImGui::EndChildFrame();
	ImGui::EndChild(); // ItemsBG
	fontText->Scale = 1.f;
	ImGui::PopFont();
	
	ImGui::PopStyleVar(); // FramePadding
	
	// Footer
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { padding, 4.f });
	ImGui::BeginChildFrame(10003, { 0.f, 30.f }, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
	ImGui::PushFont(fontTitle);
	ImGui::Text("OUTCOME");
	//GUITools::AlignRight("0 - 90", ImGui::GetWindowContentRegionMax().x);
	ImGui::PopFont();
	ImGui::EndChildFrame();
	ImGui::PopStyleVar();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { padding, padding });
	// Outcome Results
	//fontText->Scale = 1.1f;
	ImGui::PushFont(fontText);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(80, 80, 80, 40));
	ImGui::BeginChild("TradeInResBG", { 0, 0 });
	ImGui::PopStyleColor();
	ImGui::BeginChildFrame(100004, { 0, 0 }, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize);
	RenderOutcome();
	ImGui::EndChildFrame();
	ImGui::EndChild(); // TradeInResBG
	//fontText->Scale = 1.f;
	ImGui::PopFont();

	ImGui::EndChildFrame();
	ImGui::PopStyleVar();
}

std::vector<TradeIn::Series> TradeIn::UpdateSeries(const int& id)
{
	// Check if series contains our series already.
	auto it = std::find_if(series.begin(), series.end(), [&](Series const& serie) { return serie.id == id; });
	if (it != series.end())
	{
		// Add a new item to this series, later on calculate the chances
		it->itemCount++;
	}
	else {
		// A new series to this trade in, get the items for this series
		auto newSeries = itemSeriesDatabase.GetSeriesItems(id);
		// To add room for series outcome items in render
		/* DISABLE FOR NOW - Publish on later version
		additionalHeight += std::count_if
		(
			newSeries.items.begin(), 
			newSeries.items.end(), 
			[=](ItemSeriesDatabaseWrapper::SeriesItem i) { return  i.quality == quality; }
		) * 17; // Currently not tied to font size!
		*/
		series.push_back({ id, 1, newSeries });
	}
	// Sort the series by the amount of items in trade aka the chances of getting it
	std::sort(series.begin(), series.end());

	return series;
}

void TradeIn::RenderItems()
{
	float maxRight = ImGui::GetWindowContentRegionMax().x;

	for (const auto& item : items)
	{
		ImGui::TextColored({ ImColor(113, 148, 242, 255) }, "%s", item.label.c_str()); ImGui::SameLine(renderColums.at(1));
		if (item.paintLabel != "")
		{
			COLOR c = item.paint;
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(c.r, c.g, c.b, c.a));
			ImGui::SmallButton(" ");
			ImGui::PopStyleColor();
		}
		else ImGui::Text(" ");
		ImGui::SameLine(renderColums.at(1));
		GUITools::AlignRight(std::to_string(item.min), maxRight - renderColums.at(3));
		GUITools::AlignRight("-", maxRight - renderColums.at(4));
		GUITools::AlignRight(std::to_string(item.max), maxRight);
	}
}

void TradeIn::RenderOutcome()
{
	float maxRight = ImGui::GetWindowContentRegionMax().x;
	ImColor textColor = { 113, 148, 242, 255 };
	ImColor testColor = { 250, 64, 64, 255 };
	for (const auto& s : series)
	{
		int chance = (double)s.itemCount / items.size() * 100; // Avoid integer math
		ImGui::TextColored({ textColor },"%d%% %s", chance, s.data.name.c_str());

		/* DISABLE FOR NOW - Reveal on later version
		for (auto item : s.data.items)
		{
			if (item.quality == quality) 
			{
				ImGui::NewLine(); ImGui::SameLine(16.f);
				ImGui::TextColored({ testColor }, "%s", item.label.c_str());
				ImGui::SameLine(renderColums.at(1));
				// Risky on the ITEMPAIN::DEFAULT - store these inside an pop-up, with loop on all paints?
				GUITools::AlignRight(std::to_string(item.price[ITEMPAINT::DEFAULT].min), maxRight - renderColums.at(3));
				GUITools::AlignRight("-", maxRight - renderColums.at(4));
				GUITools::AlignRight(std::to_string(item.price[ITEMPAINT::DEFAULT].max), maxRight);
			}
		}
		*/
	}
}

void TradeIn::CalculateRenderInfo()
{
	// As we do a bit wonky alignment, we'll need to use some sort of padding between items
	float pad = 13.f;
	float name{}, paint{}, min{}, max{};
	// Loop items, and find longest words of each column
	for (const auto& item : items)
	{
		float i_name = ImGui::CalcTextSize(item.label.c_str()).x,
			i_paint = ImGui::CalcTextSize(item.paintLabel.c_str()).x,
			i_min = ImGui::CalcTextSize(std::to_string(item.min).c_str()).x,
			i_max = ImGui::CalcTextSize(std::to_string(item.max).c_str()).x;

		name = i_name > name ? i_name : name;
		paint = i_paint > paint ? i_paint : paint;
		min = i_min > min ? i_min : min;
		max = i_max > max ? i_max : max;
	}
	// Temp fix - not using paint names, only a small blib
	paint = 15.f;
	// Calc whole window width, adding padding between columns
	renderWidth = name + paint + min + max + (pad * 6);

	// Min width is 250, check if we are under it.
	float diff = 250.f - renderWidth;
	if (diff > 0)
	{
		renderWidth = 250.f;
		name += diff / 1.5f;
	}
	// Vector should have these fields - as its initialized on class creation.
	renderColums[0] = (name + pad);
	renderColums[1] = (name + paint + (pad * 2));
	renderColums[2] = (max + min);
	renderColums[3] = (max + ImGui::CalcTextSize("-").x + 10.f);
	renderColums[4] = (max + 5.f);
}
