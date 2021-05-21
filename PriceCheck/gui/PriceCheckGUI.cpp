#include "pch.h"
#include "IMGUI/imgui_internal.h";
#include "PriceCheck.h"
#include "bakkesmod/wrappers/GuiManagerWrapper.h"
#include "gui/GUITools.h"

// Do ImGui rendering here
void PriceCheck::Render()
{
	if (*forceShow)
	{
		// Show everything
		DrawTradeWindow();
		DrawTradeInWindow();
	}
	if (guiState.showTrade)
	{
		DrawTradeWindow();
	}

	if (guiState.showTradeIn)
	{
		DrawTradeInWindow();
	}

	return;
	/*
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, 
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// ImGui renreding ends here
	ImGui::End();
	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
	*/
}

void PriceCheck::DrawTradeWindow()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.25f);
	// IM_COL32(5, 10, 19, 255)
	// IM_COL32(21, 50, 85, 255)
	// IM_COL32(41, 68, 118, 255)
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(36, 62, 110, 255));
	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(99, 121, 157, 255));

	playerTrade.Render(fonts, &guiState.showTrade);

	// End - Window render stops here
	ImGui::End();
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);
}

void PriceCheck::DrawTradeInWindow()
{
	//ImGui::ShowDemoWindow();
	/* 
	* Define window style 
	* Border, Rounding, Padding, Colors
	*/
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.5f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f, 0.f });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(5, 10, 19, 255)); // Colors are taken from Music Widget
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.286f, 0.505f, 0.709f, 0.9f }); 
	/* Is this needed, as seems that ImGui will save the screen positions */
	tradeIn.Render(fonts, &guiState.showTrade);

	// End - Window render stops here
	ImGui::End();
	ImGui::PopStyleVar(3);
	ImGui::PopStyleColor(2);
}

// Name of the menu that is used to toggle the window.
std::string PriceCheck::GetMenuName()
{
	return "PriceCheck";
}

// Title to give the menu
std::string PriceCheck::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void PriceCheck::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
	auto gui = gameWrapper->GetGUIManager();
	// How to know if an font is loaded?
	fonts.LoadFonts(gameWrapper);
}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool PriceCheck::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool PriceCheck::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void PriceCheck::OnOpen()
{
	isWindowOpen_ = true;
}

// Called when window is closed
void PriceCheck::OnClose()
{
	isWindowOpen_ = false;
}