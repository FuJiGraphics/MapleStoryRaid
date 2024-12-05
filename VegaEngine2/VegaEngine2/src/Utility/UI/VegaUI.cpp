#include "pch.h"
#include "VegaUI.h"

namespace fz {
	bool VegaUI::DrawControl1(const std::string& label, const std::string& buttonLabel, int& v, int speed_x, int x_min, int x_max, int resetValue, float columnWidth)
	{
		bool result = false;
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 4.f });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight * 1.81f, lineHeight };

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f });
		if (ImGui::Button(buttonLabel.c_str(), buttonSize))
		{
			v = resetValue;
			result = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragInt("##y", &v, speed_x, x_min, x_max))
			result = true;

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
		return result;
	}
	bool VegaUI::DrawControl1(const std::string& label,
							  const std::string& buttonLabel,
							  float& v,
							  float speed_x, float x_min, float x_max,
							  float resetValue, float columnWidth)
	{
		bool result = false;
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 4.f });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight * 1.81f, lineHeight };

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f });
		if (ImGui::Button(buttonLabel.c_str(), buttonSize))
		{
			v = resetValue;
			result = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragFloat("##y", &v, speed_x, x_min, x_max))
			result = true;

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
		return result;
	}

	bool VegaUI::DrawControl2(const std::string& label, sf::Vector2f& vec2,
									 float speed_x, float speed_y,
									 float x_min, float x_max,
									 float y_min, float y_max,
									 float resetValue, float columnWidth)
	{
		bool result = false;
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight, lineHeight };

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
		{
			vec2.x = resetValue;
			result = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragFloat("##x", &vec2.x, speed_x, x_min, x_max))
			result = true;
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
		{
			vec2.y = resetValue;
			result = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragFloat("##y", &vec2.y, speed_y, y_min, y_max))
			result = true;

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
		return result;
	}

	bool VegaUI::DrawControl4(const std::string& label, sf::IntRect& rect, 
							  const sf::IntRect& rectSpeed, 
							  const sf::IntRect& rectMin, const sf::IntRect& rectMax,
							  const sf::IntRect& resetValues, float columnWidth)
	{
		bool result = false;
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight, lineHeight };

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
		{
			rect.left = resetValues.left;
			result = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragInt("##left", &rect.left, rectSpeed.left, rectMin.left, rectMax.left))
			result = true;
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
		{
			rect.top = resetValues.top;
			result = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragInt("##top", &rect.top, rectSpeed.top, rectMin.top, rectMax.top))
			result = true;
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("W", buttonSize))
		{
			rect.width = resetValues.width;
			result = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragInt("##width", &rect.width, rectSpeed.width, rectMin.width, rectMax.width))
			result = true;
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("H", buttonSize))
		{
			rect.height = resetValues.height;
			result = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragInt("##height", &rect.height, rectSpeed.height, rectMin.height, rectMax.height))
			result = true;

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
		return result;
	}


	bool VegaUI::ColorEdit4(sf::Color& dst, const std::string& label)
	{
		const char* newLabel = "##Empty";

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100.f);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		// rect 초기화
		auto& [r, g, b, a] = dst;  // dst가 sf::Color라고 가정
		std::vector<float> rect(4, 0.0f);
		rect[0] = r / 255.0f;
		rect[1] = g / 255.0f;
		rect[2] = b / 255.0f;
		rect[3] = a / 255.0f;

		bool result = false;
		if (ImGui::ColorEdit4(newLabel, rect.data()))
		{
			r = (sf::Uint8)(rect[0] * 255);
			g = (sf::Uint8)(rect[1] * 255);
			b = (sf::Uint8)(rect[2] * 255);
			a = (sf::Uint8)(rect[3] * 255);
			result = true;
		}

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
		return result;
	}

	bool VegaUI::ColorEdit4WidthCheckbox(sf::Color& dst, bool& check, const std::string& label)
	{
		const char* newLabel = "##Empty";

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100.f);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		// rect 초기화
		auto& [r, g, b, a] = dst;  // dst가 sf::Color라고 가정
		std::vector<float> rect(4, 0.0f);
		rect[0] = r / 255.0f;
		rect[1] = g / 255.0f;
		rect[2] = b / 255.0f;
		rect[3] = a / 255.0f;

		bool result = false;
		if (ImGui::ColorEdit4(newLabel, rect.data()))
		{
			r = (sf::Uint8)(rect[0] * 255);
			g = (sf::Uint8)(rect[1] * 255);
			b = (sf::Uint8)(rect[2] * 255);
			a = (sf::Uint8)(rect[3] * 255);
			if (check)
				result = true;
		}

		ImGui::SameLine();
		if (ImGui::Checkbox("##Mask", &check))
		{
			result = true;
		}
		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
		return result;
	}

	bool VegaUI::InputText(const std::string& label, std::string& dst, float columnWidth)
	{
		bool result = false;
		ImGui::PushID(("Input Text" + label).c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight * 2.0f, lineHeight };
		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());

		char c[256];
		strcpy_s(c, sizeof(c), dst.c_str());
		if (ImGui::InputText("##InputText", c, IM_ARRAYSIZE(c)))
		{
			dst = c;
			result = true;
		}

		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopID();
		return result;
	}

	bool VegaUI::InputFloat(float& dst, const std::string& label, bool isRightLabel, float step, float step_fast)
	{
		float f = 0.0f;
		f = dst;

		const char* newLabel = "##Empty";
		if (!label.empty())
		{
			if (isRightLabel)
				newLabel = label.c_str();
			else
				newLabel = VegaUI::LabelPrefix(label);
		}

		bool result = false;
		if (ImGui::InputFloat(newLabel, &f, step, step_fast))
		{
			dst = f;
			result = true;
		}
		return result;
	}

	bool VegaUI::InputFloat2(sf::Vector2f& dst, const std::string& label, bool isRightLabel)
	{
		float f[2] = { 0.0f, 0.0f };
		f[0] = dst.x;
		f[1] = dst.y;

		const char* newLabel = "##Empty";
		if (!label.empty())
		{
			if (isRightLabel)
				newLabel = label.c_str();
			else
				newLabel = VegaUI::LabelPrefix(label);
		}

		bool result = false;
		if (ImGui::InputFloat2(newLabel, f))
		{
			dst.x = f[0];
			dst.y = f[1];
			result = true;
		}
		return result;
	}

	bool VegaUI::DragFloat(float& dst, const std::string& label, bool isRightLabel, float v_speed, float v_min, float v_max)
	{
		float f = 0.0f;
		f = dst;

		const char* newLabel = "##Empty";
		if (!label.empty())
		{
			if (isRightLabel)
				newLabel = label.c_str();
			else
				newLabel = VegaUI::LabelPrefix(label);
		}

		bool result = false;
		if (ImGui::DragFloat(newLabel, &f, v_speed, v_min, v_max))
		{
			dst = f;
			result = true;
		}
		return result;
	}

	bool VegaUI::DragFloat2(sf::Vector2f& dst, const std::string& label, bool isRightLabel, float v_speed, float v_min, float v_max)
	{
		float f[2] = { 0.0f, 0.0f };
		f[0] = dst.x;
		f[1] = dst.y;

		const char* newLabel = "##Empty";
		if (!label.empty())
		{
			if (isRightLabel)
				newLabel = label.c_str();
			else
				newLabel = VegaUI::LabelPrefix(label);
		}

		bool result = false;
		if (ImGui::DragFloat2(newLabel, f, v_speed, v_min, v_max))
		{
			dst.x = f[0];
			dst.y = f[1];
			result = true;
		}
		return result;
	}

	bool VegaUI::DragFloat4(sf::FloatRect& dst, const std::string& label, bool isRightLabel, float v_speed, float v_min, float v_max)
	{
		float f[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		f[0] = dst.left;
		f[1] = dst.top;
		f[2] = dst.width;
		f[3] = dst.height;
		const char* newLabel = "##Empty";
		if (!label.empty())
		{
			if (isRightLabel)
				newLabel = label.c_str();
			else
				newLabel = VegaUI::LabelPrefix(label);
		}

		bool result = false;
		if (ImGui::DragFloat4(newLabel, f, v_speed, v_min, v_max))
		{
			dst.left = f[0];
			dst.top = f[1];
			dst.width = f[2];
			dst.height = f[3];
			result = true;
		}
		return result;
	}

	const char* VegaUI::LabelPrefix(const std::string& label)
	{
		ImGui::Text(label.c_str());
		ImGui::SameLine();

		static std::string labelID = "";
		labelID = "##" + label;

		return labelID.c_str();
	}

	void VegaUI::SelectOrigins(fz::Sprite& sprite, float columnWidth)
	{
		ImGui::PushID("Origins");
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("Origins");
		ImGui::NextColumn();

		const char* items[] = { "Custom", "TopLeft", "TopCenter", "TopRight", "MidLeft", "MidCenter", "MidRight", "BotLeft", "BotCenter", "BotRight" };
		Origins item_selected_origins = sprite.GetOrigins();

		int preview = (item_selected_origins == Origins::Custom) ? 0 : (int)(item_selected_origins)+1;
		const char* combo_preview_value = items[preview];
		if (ImGui::BeginCombo("##Origins", combo_preview_value))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = false;
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_selected_origins = (n == 0) ? Origins::Custom : (Origins)(n - 1);
					is_selected = true;
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
					sprite.SetOrigins(item_selected_origins);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	bool VegaUI::PopupContextItem(const std::string& item, const std::function<void()>& fn)
	{
		bool result = false;
		if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem(item.c_str()))
			{
				fn();
				result = true;
			}
			ImGui::EndPopup();
		}
		return result;
	}

	bool VegaUI::DragRect(sf::Vector2f& start, sf::Vector2f& end, bool currentWindow, ImGuiMouseButton button)
	{
		bool result = false;
		static ImVec2 startPos, endPos;
		ImVec2 winPos = ImGui::GetWindowPos(); 
		float titleBarH = ImGui::GetFrameHeight();
		ImVec2 currPos = ImGui::GetMousePos();
		if (ImGui::IsMouseClicked(button))
		{
			startPos = ImGui::GetMousePos();
			float dx = startPos.x;
			float dy = startPos.y;
			if (currentWindow) 
			{
				dx -= winPos.x;
				dy -= winPos.y - titleBarH;
			}
			if (dx > 0.0f && dy > 0.0f)
				start = { dx, dy };
			else
				start = { -1.0f, -1.0f };
		}
		if (VegaUI::IsWindowFocused({ currPos.x, currPos.y }) && ImGui::IsMouseDown(button))
		{
			endPos = ImGui::GetMousePos();
			ImDrawList* draw_list = ImGui::GetForegroundDrawList();
			draw_list->AddRect(startPos, endPos, ImGui::GetColorU32(IM_COL32(0, 130, 216, 255)));
		}
		if (VegaUI::IsWindowFocused({ currPos.x, currPos.y }) && ImGui::IsMouseReleased(button))
		{
			result = true;
			float dx = endPos.x;
			float dy = endPos.y;
			if (currentWindow)
			{
				dx -= winPos.x;
				dy -= winPos.y - titleBarH;
			}
			if (dx > 0.0f && dy > 0.0f)
				end = { dx, dy };
			else
				end = { -1.0f, -1.0f };
		}
		return result;
	}

	bool VegaUI::IsWindowFocused(const sf::Vector2f& pos)
	{
		ImVec2 winPos = ImGui::GetWindowPos();
		float titleBarH = ImGui::GetFrameHeight();
		const auto& winSize = ImGui::GetWindowSize();
		if (pos.x < winPos.x || pos.y < winPos.y + titleBarH)
			return false;
		if ((pos.x > (winPos.x + winSize.x)) || (pos.y > (winPos.y + winSize.y + titleBarH)))
			return false;
		return true;
	}

	void VegaUI::DrawGrid(float gridSize, const sf::FloatRect& borderRect)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		const auto& winPos = ImGui::GetWindowPos();
		const auto& winSize = ImGui::GetWindowSize();
		const auto& frameHeight = ImGui::GetFrameHeight();
		ImU32 gridColor = ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 0.5f));

		draw_list->AddRectFilled(
			ImVec2(winPos.x, winPos.y + frameHeight),
			ImVec2(winPos.x + winSize.x, winPos.y + winSize.y),
			ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 0.5f))
		);
		for (float y = winPos.y + frameHeight + borderRect.top; y < winPos.y + winSize.y; y += gridSize) {
			draw_list->AddLine(ImVec2(winPos.x + borderRect.left, y), ImVec2(winPos.x + winSize.x - borderRect.width, y), gridColor);
		}

		for (float x = winPos.x + borderRect.left; x < winPos.x + winSize.x; x += gridSize) {
			draw_list->AddLine(ImVec2(x, winPos.y + frameHeight + borderRect.top), ImVec2(x, winPos.y + winSize.y - borderRect.height), gridColor);
		}
	}

	bool VegaUI::Checkbox(const std::string& label, bool& check, float columnWidth)
	{
		bool result = false;
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 4.f });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight * 1.81f, lineHeight };

		// ImGui::SameLine();
		if (ImGui::Checkbox(("##CheckBox" + label).c_str(), &check))
			result = true;

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
		return result;
	}

} // namespace fz