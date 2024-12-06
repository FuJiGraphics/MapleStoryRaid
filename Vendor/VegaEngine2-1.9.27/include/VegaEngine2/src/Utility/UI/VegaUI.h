#pragma once
#include <imgui.h>
#include <imgui_internal.h>

namespace fz {

	class VegaUI
	{
	public:
		static bool DrawControl1(const std::string& label, const std::string& buttonLabel,
								 int& v,
								 int speed_x = 1,
								 int x_min = 0, int x_max = 0,
								 int resetValue = 0, float columnWidth = 100.f);
		static bool DrawControl1(const std::string& label, const std::string& buttonLabel,
								 float& v,
								 float speed_x = 0.1f,
								 float x_min = 0.0f, float x_max = 0.0f,
								 float resetValue = 0.0f, float columnWidth = 100.f);
		static bool DrawControl2(const std::string& label, sf::Vector2f& vec2,
								 float speed_x = 0.1f, float speed_y = 0.1f,
								 float x_min = 0.0f, float x_max = 0.0f,
								 float y_min = 0.0f, float y_max = 0.0f,
								 float resetValue = 0.0f, float columnWidth = 100.f);
		static bool DrawControl4(const std::string& label, sf::IntRect& rect,
								 const sf::IntRect& rectSpeed = { 1, 1, 1, 1 },
								 const sf::IntRect& rectMin = { 0, 0, 0, 0 }, const sf::IntRect& rectMax = { 0, 0, 0, 0 },
								 const sf::IntRect& resetValues = { 0, 0, 0, 0 }, float columnWidth = 100.f);
		static bool ColorEdit4(sf::Color& dst, const std::string& label);
		static bool ColorEdit4WidthCheckbox(sf::Color& dst, bool& check, const std::string& label);
		static bool InputFloat(float& dst, const std::string& label = "", bool isRightLabel = false, float step = 0.0f, float step_fast = 0.0f);
		static bool InputFloat2(sf::Vector2f& dst, const std::string& label = "", bool isRightLabel = false);
		static bool DragFloat(float& dst, const std::string& label = "", bool isRightLabel = false, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f);
		static bool DragFloat2(sf::Vector2f& dst, const std::string& label = "", bool isRightLabel = false, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f);
		static bool DragFloat4(sf::FloatRect& dst, const std::string& label = "", bool isRightLabel = false, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f);
		static const char* LabelPrefix(const std::string& label);
		static std::string OpenFile(HWND hwnd, const char* filter)
		{
			namespace fs = std::filesystem;
			OPENFILENAMEA ofn;
			CHAR szFile[260] = { 0 };
			CHAR currentDir[256] = { 0 };
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFile = szFile; 
			ofn.nMaxFile = sizeof(szFile);
			if (GetCurrentDirectoryA(256, currentDir)) 
				ofn.lpstrInitialDir = currentDir;
			ofn.lpstrFilter = filter; 
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
			if (GetOpenFileNameA(&ofn) == TRUE) {
				fs::path absolutePath = ofn.lpstrFile;
				fs::path currentPath = fs::current_path();
				fs::path relativePath = fs::relative(absolutePath, currentPath);
				return relativePath.string();
			}
			return std::string();
		}

		static std::string SaveFile(HWND hwnd, const char* filter)
		{
			OPENFILENAMEA ofn;
			CHAR szFile[260] = { 0 };
			CHAR currentDir[256] = { 0 };
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			if (GetCurrentDirectoryA(256, currentDir))
				ofn.lpstrInitialDir = currentDir;
			ofn.lpstrFilter = filter;
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

			// Sets the default extension by extracting it from the filter
			ofn.lpstrDefExt = strchr(filter, '\0') + 1;

			if (GetSaveFileNameA(&ofn) == TRUE)
				return ofn.lpstrFile;

			return std::string();
		}

		static bool OpenTextureFile(HWND handle, std::string& path, float columnWidth = 100.f)
		{
			bool result = false;
			ImGui::PushID("Texture Path");
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::Text("Texture Path");
			ImGui::NextColumn();

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight * 2.0f, lineHeight };
			ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());

			char c[256];
			strcpy_s(c, sizeof(c), path.c_str());
			ImGui::InputText("##InputText", c, IM_ARRAYSIZE(c));

			ImGui::SameLine();
			if (ImGui::Button("Add", buttonSize))
			{
				std::string prevPath = path;
				path = VegaUI::OpenFile(handle, "Texture File (*.png)\0*.png\0");
				if (path.empty())
					path = prevPath;
				result = true;
			}

			ImGui::PopItemWidth();
			ImGui::Columns(1);
			ImGui::PopID();
			return result;
		}
		static bool OpenFontFile(HWND handle, std::string& path, float columnWidth = 100.f)
		{
			bool result = false;
			ImGui::PushID("Font Path");
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::Text("Font Path");
			ImGui::NextColumn();

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight * 2.0f, lineHeight };
			ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());

			char c[256];
			strcpy_s(c, sizeof(c), path.c_str());
			ImGui::InputText("##InputText", c, IM_ARRAYSIZE(c));

			ImGui::SameLine();
			if (ImGui::Button("Add", buttonSize))
			{
				std::string prevPath = path;
				path = VegaUI::OpenFile(handle, "Font File (*.ttf)\0*.ttf\0");
				if (path.empty())
					path = prevPath;
				result = true;
			}

			ImGui::PopItemWidth();
			ImGui::Columns(1);
			ImGui::PopID();
			return result;
		}
		static bool InputText(const std::string& label, std::string& dst, float columnWidth = 100.f);
		static void SelectOrigins(fz::Sprite& sprite, float columnWidth = 100.f);
		static bool PopupContextItem(const std::string& item, const std::function<void()>& fn);
		static bool DragRect(sf::Vector2f& start, sf::Vector2f& end, bool currentWindow = true, ImGuiMouseButton button = ImGuiMouseButton_Left);
		static bool IsWindowFocused(const sf::Vector2f& pos);
		static void DrawGrid(float gridSize, const sf::FloatRect& borderRect = { 0.0f, 0.0f, 0.0f, 0.0f });
		static bool Checkbox(const std::string& label, bool& check, float columnWidth = 100.f);
	};

} // namespace fz
