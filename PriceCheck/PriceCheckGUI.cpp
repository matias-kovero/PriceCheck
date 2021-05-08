#include "pch.h"
#include "PriceCheck.h"


// Do ImGui rendering here
void PriceCheck::Render()
{
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}
	//ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_NoTitleBar);
	static bool cb = false;
	ImGui::Checkbox("Toggle", &cb);

	// ImGui renreding ends here
	ImGui::End();
	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
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
	//ImGui::SetNextWindowBgAlpha(1);
	isWindowOpen_ = true;
	//ImGuiStyle& style = ImGui::GetStyle();
	//auto& c = style.Colors[ImGuiCol_WindowBg];
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.7f));
	ImGui::PushStyleVar(ImGuiWindowFlags_NoTitleBar, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(-1, -1));
	//ImGuiStyleVar_
	//ImGuiStyle& style = ImGui::GetStyle();
	//style.Colors[ImGuiCol_WindowBg] = ImColor(30, 30, 30, 50);
	//ImGui::SetNextWindowBgAlpha(1);
}

// Called when window is closed
void PriceCheck::OnClose()
{
	isWindowOpen_ = false;
}