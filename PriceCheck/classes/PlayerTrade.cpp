#include "pch.h"
#include "TradeItem.h"
#include "PlayerTrade.h"
#include "gui/GUITools.h"

PlayerTrade::PlayerTrade()
{
}

PlayerTrade::~PlayerTrade()
{
}

void PlayerTrade::CheckTrade(TradeWrapper trade)
{
	ArrayWrapper<OnlineProductWrapper> me = trade.GetSendingProducts();
	ArrayWrapper<OnlineProductWrapper> him = trade.GetReceivingProducts();

	std::vector<TradeWrapper::Currency> meCurrency = trade.GetSendingCurrency();
	std::vector<TradeWrapper::Currency> himCurrency = trade.GetReceivingCurrency();

	Clear();

	AddItems(me, sendValue);
	AddItems(him, recvValue);

	AddCredits(meCurrency, sendValue);
	AddCredits(himCurrency, recvValue);

	// Update our render info
	CalculateColumns();
}

void PlayerTrade::Clear()
{
	sendValue = Value();
	recvValue = Value();
}

void PlayerTrade::Render(Fonts fonts, bool show)
{
	if (!fontTitle) fontTitle = fonts.GetFont("RLHeadI");
	//if (!fontSmallTitle) fontSmallTitle = fonts.GetFont("RLHead");
	if (!fontText) fontText = fonts.GetFont("default");

	ImGui::PushFont(fontTitle);

	// Calculate window size
	ImVec2 box_size = { renderWidth, ImGui::GetTextLineHeight() * 3 + (padding * 4) }; // 3 lines of text, 4 caps
	ImGui::SetNextWindowSize(box_size);
	// Yeet out for optimization
	if (!ImGui::Begin(menuName.c_str(), &show,
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize))
	{
		// Early out
		ImGui::PopFont();
		return;
	}
	float maxRight = ImGui::GetWindowContentRegionMax().x;
	// Start rendering content
	GUITools::BoxShadow(ImGui::GetWindowPos(), ImGui::GetWindowSize(), 1.1f, true);

	int rSum = (recvValue.min + recvValue.max) / 2;
	int gSum = (sendValue.min + sendValue.max) / 2;
	string state = "RESULT:";
	rSum == gSum ? state = "RESULT:" :
		rSum > gSum ? state = "PROFIT:" : state = "LOSS:";
	if (useAvg) 
	{
		ImGui::Text("SENDING");
		GUITools::AlignRight(std::to_string(gSum), maxRight);
		ImGui::Text("RECEIVING");
		GUITools::AlignRight(std::to_string(rSum), maxRight);
	}
	else 
	{
		ImGui::Text("SENDING");
		GUITools::AlignRight(std::to_string(sendValue.min), maxRight - renderColums.at(2));
		GUITools::AlignRight("-", maxRight - renderColums.at(3));
		GUITools::AlignRight(std::to_string(sendValue.max), maxRight);
		ImGui::Text("RECEIVING");
		GUITools::AlignRight(std::to_string(recvValue.min), maxRight - renderColums.at(2));
		GUITools::AlignRight("-", maxRight - renderColums.at(3));
		GUITools::AlignRight(std::to_string(recvValue.max), maxRight);
	}

	ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(255, 255, 255, 70));
	ImGui::Separator();
	ImGui::PopStyleColor();

	if (rSum != gSum)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ rSum > gSum ? colorProfit : colorLoss } );
	}

	ImGui::Text(state.c_str());
	if (useAvg)
	{
		GUITools::AlignRight(std::to_string(rSum - gSum), maxRight);
	}
	else 
	{ 
		GUITools::AlignRight(std::to_string(std::abs(recvValue.min - sendValue.min)), maxRight - renderColums.at(2));
		GUITools::AlignRight("-", maxRight - renderColums.at(3));
		GUITools::AlignRight(std::to_string(std::abs(recvValue.max - sendValue.max)), maxRight);
	}
	if (rSum != gSum) ImGui::PopStyleColor();

	ImGui::PopFont();
}

void PlayerTrade::AddItems(ArrayWrapper<OnlineProductWrapper> items, Value& trader)
{
	for (TradeItem i : items)
	{
		PaintPrice price = i.GetPrice();
		trader.min += price.min;
		trader.max += price.max;
	}
}

void PlayerTrade::AddCredits(const std::vector<TradeWrapper::Currency>& currency, Value& trader)
{
	for (const auto& c : currency)
	{
		if (c.currency_id == Currencies::Credits)
		{
			trader.min += c.quantity;
			trader.max += c.quantity;
		}
	}
}

void PlayerTrade::CalculateColumns()
{
	float pad = 12.f;
	float title = 75.f, min{}, max{};

	if (useAvg)
	{
		int rSum = (recvValue.min + recvValue.max) / 2;
		int gSum = (sendValue.min + sendValue.max) / 2;
		max = rSum > gSum ? ImGui::CalcTextSize(std::to_string(rSum).c_str()).x : ImGui::CalcTextSize(std::to_string(gSum).c_str()).x;
	}
	else 
	{
		// Calculate longest: min & max
		min = sendValue.min > recvValue.min ? 
			ImGui::CalcTextSize(std::to_string(sendValue.min).c_str()).x : 
			ImGui::CalcTextSize(std::to_string(recvValue.min).c_str()).x;
		max = sendValue.max > recvValue.max ?
			ImGui::CalcTextSize(std::to_string(sendValue.max).c_str()).x :
			ImGui::CalcTextSize(std::to_string(recvValue.max).c_str()).x;
	}
	if (max < 1.f) // For some reason, it might sometimes return 0?
	{
		max = 32.f;
		min = 32.f;
	}
	// Calc whole window width, adding padding between columns
	renderWidth = title + min + max + (pad * 6);

	// Min width is 190, check if we are under it.
	float diff = 190.f - renderWidth;
	if (diff > 0)
	{
		renderWidth = 190.f;
		title += diff / 1.5f;
	}
	// Should figure a better way to do this, a bit risky as if configured wrong will be out_of_range
	renderColums[0] = title + pad;
	renderColums[1] = max + min;
	renderColums[2] = max + ImGui::CalcTextSize("-").x + pad * 1.75;
	renderColums[3] = max + pad;
}
