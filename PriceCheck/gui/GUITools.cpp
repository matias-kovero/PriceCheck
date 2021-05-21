#include "pch.h"
#include "GUITools.h"
#include "IMGUI/imgui_internal.h"

void GUITools::BoxShadow(ImVec2 pos, ImVec2 size, float gap, bool dark)
{
	std::vector<ImColor> cDark =
	{
		ImColor(0, 0, 0, 5),
		ImColor(0, 0, 0, 10),
		ImColor(0, 0, 0, 20),
		ImColor(0, 0, 0, 30),
		ImColor(0, 0, 0, 40)
	};
	std::vector<ImColor> cLight =
	{
		ImColor(73, 129, 181, 5),
		ImColor(73, 129, 181, 10),
		ImColor(73, 129, 181, 30),
		ImColor(73, 129, 181, 50),
		ImColor(73, 129, 181, 60),
	};

	float shift = cLight.size() * gap;
	pos -= {shift, shift};
	size += {shift * 2, shift * 2};

	for (const auto& c : dark ? cDark : cLight)
	{
		ImGui::GetBackgroundDrawList()->AddRect(
			pos, pos + size, c, 1.0f, 15, gap
		);
		pos += { gap, gap };
		size -= { gap * 2, gap * 2 };
	}
}

void GUITools::AlignRight(string text, float posX)
{
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;
	//ImGui::SetCursorPosX(posX + ImGui::GetColumnWidth() - textWidth);
	ImGui::SameLine(posX - textWidth, 0.0);

	/*
	ImVec2 cPos = ImGui::GetCursorPos();
	ImVec2 wPos = ImGui::GetWindowPos();

	ImGui::GetWindowDrawList()->AddRect(wPos + cPos,
		{ wPos.x + cPos.x + textWidth, wPos.y + cPos.y + ImGui::GetTextLineHeight() },
		IM_COL32(255, 255, 0, 70)
	);
	*/
	ImGui::Text("%s", text.c_str());
}
