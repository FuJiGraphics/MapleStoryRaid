#include "SpriteEditor.h"

namespace fz {

	namespace {
		static sf::Vector2f s_ButtonSize;
		static sf::Sprite s_MoveTex;
		static sf::Sprite s_CropTex;
		static sf::RectangleShape s_DrawRect;
		static sf::Vector2f s_BaseSize = { 1.0f, 1.0f };
		static sf::Vector2f s_CurrentDrawSize = { 200.0f, 200.0f };
		static int s_IsClickedFrame = -1;
		static bool s_HasCrop = false;
		static std::string s_ClipName = "None";
		static AnimationLoopTypes s_LoopType = AnimationLoopTypes::Loop;
		static std::string s_OpenFilePath = "";
		static std::string s_CurrentPath = "";
		static sf::Vector2f s_Translate = { 0.0f, 0.0f };
		static sf::Vector2f s_Scale = { 1.0f, 1.0f };
		static float s_Rotation = 0.0f;
		static std::string ToString(AnimationLoopTypes type)
		{
			switch(type)
			{
				case AnimationLoopTypes::Single:
					return "Single";
				case AnimationLoopTypes::Loop:
					return "Loop";
				case AnimationLoopTypes::PingPong:
					return "PingPong";
			}
			FZLOG_ASSERT(false, "Failed to converted a type");
		}
		static AnimationLoopTypes ToLoopType(const std::string& str)
		{
			if (str == "Single")
				return AnimationLoopTypes::Single;
			if (str == "Loop")
				return AnimationLoopTypes::Loop;
			if (str == "PingPong")
				return AnimationLoopTypes::PingPong;
			FZLOG_ASSERT(false, "Failed to converted a type");
			return AnimationLoopTypes::Single;
		}
	}

	EditorCamera SpriteEditor::s_EditorCamera({ 100.f, 100.f }, false);
	void SpriteEditor::ClampCameraBounds()
	{
		sf::Vector2f cameraCenter = s_EditorCamera.GetOrthoCamera().GetCenter();
		sf::Vector2f cameraSize = s_EditorCamera.GetOrthoCamera().GetSize();
		sf::Vector2f framebufferSize = { (float)s_FrameBuffer->GetWidth(), (float)s_FrameBuffer->GetHeight() };

		sf::Vector2f minBounds = cameraSize / 2.0f; 
		sf::Vector2f maxBounds = framebufferSize - (cameraSize / 2.0f); 

		if (minBounds.x < maxBounds.x)
			cameraCenter.x = std::clamp(cameraCenter.x, minBounds.x, maxBounds.x);
		if (minBounds.y < maxBounds.y)
			cameraCenter.y = std::clamp(cameraCenter.y, minBounds.y, maxBounds.y);

		s_EditorCamera.GetOrthoCamera().SetCenter(cameraCenter);
	}

	void SpriteEditor::ClampCameraZoom()
	{
		static float currentZoom = 1.0f;
		const float minZoom = 0.5f;
		const float maxZoom = 2.0f;

		currentZoom = std::clamp(currentZoom, minZoom, maxZoom);
		s_EditorCamera.GetOrthoCamera().SetSize(s_BaseSize.x * currentZoom, s_BaseSize.y * currentZoom);
		ClampCameraBounds();
	}

	void SpriteEditor::UpdateFrames()
	{
		ImGui::Begin("Frames", 0, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
			if( ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("New..."))
				{
					auto nativeWindow = (sf::RenderWindow*)System::GetSystem().GetWindow().GetNativeWindow();
					HWND handle = (HWND)nativeWindow->getSystemHandle();
					std::string openPath = VegaUI::OpenFile(handle, "Atlas Image (*.png)\0*.png\0");
					if (!openPath.empty())
					{
						SpriteEditor::Clear();
						SpriteEditor::SetTarget(openPath);
						s_IsOpenedFile = true;
					}
					else
						s_IsOpenedFile = false;
				}
				if (s_IsOpenedFile && ImGui::MenuItem("Add Frame"))
				{
					s_Frames.push_back(s_WorkSprite);
					auto iter = (s_Frames.end() - 1);
					int x = static_cast<int>(s_DrawRect.getPosition().x);
					int y = static_cast<int>(s_DrawRect.getPosition().y);
					int sizeW = static_cast<int>(s_DrawRect.getSize().x);
					int sizeH = static_cast<int>(s_DrawRect.getSize().y);
					sf::IntRect rect = { x, y, sizeW, sizeH };
					iter->setTextureRect(rect);
				}
				if (ImGui::MenuItem("Open Clip"))
				{
					auto nativeWindow = (sf::RenderWindow*)System::GetSystem().GetWindow().GetNativeWindow();
					HWND handle = (HWND)nativeWindow->getSystemHandle();
					std::string openPath = VegaUI::OpenFile(handle, "Animation Clip (*.anim)\0*.anim\0");
					if (!openPath.empty())
					{
						SpriteEditor::OpenAniamtionClip(openPath);
					}
				}
				if (s_IsOpenedFile && ImGui::MenuItem("Save..."))
				{
					if (!s_CurrentPath.empty()) 
					{
						SpriteEditor::SaveAnimationClip(s_CurrentPath);
					}
					else
					{
						auto nativeWindow = (sf::RenderWindow*)System::GetSystem().GetWindow().GetNativeWindow();
						HWND handle = (HWND)nativeWindow->getSystemHandle();
						std::string savePath = VegaUI::SaveFile(handle, "Animation Clip (*.anim)\0*.anim\0");
						if (!savePath.empty())
						{
							SpriteEditor::SaveAnimationClip(savePath);
						}
					}
				}
				if (s_IsOpenedFile && ImGui::MenuItem("Save As..."))
				{
					auto nativeWindow = (sf::RenderWindow*)System::GetSystem().GetWindow().GetNativeWindow();
					HWND handle = (HWND)nativeWindow->getSystemHandle();
					std::string savePath = VegaUI::SaveFile(handle, "Animation Clip (*.anim)\0*.anim\0");
					if (!savePath.empty())
					{
						SpriteEditor::SaveAnimationClip(savePath);
					}
				}
				if (ImGui::MenuItem("Exit"))
				{
					SpriteEditor::SetActive(false);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		int deleteFrameIndex = -1;
		bool isRemove = false;
		int countOfFrames = static_cast<int>(s_Frames.size());
		for (int i = 0; i < countOfFrames; ++i)
		{
			std::string id = std::string("Frame") + std::to_string(i) + "_33";
			if (ImGui::ImageButton(id.c_str(), s_Frames[i], { 100.f, 100.f }))
			{
				s_IsClickedFrame = i;
				int width = s_Frames[i].getTextureRect().width;
				int height = s_Frames[i].getTextureRect().height;
				s_PreviewBuffer->Resize(width, height);
			}
			if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight))
			{
				if (ImGui::MenuItem("Remove"))
				{
					deleteFrameIndex = i;
					isRemove = true;
				}
				ImGui::EndPopup();
			}
		}

		if (isRemove)
		{
			s_Frames.erase(s_Frames.begin() + deleteFrameIndex);
			isRemove = false;
			deleteFrameIndex = -1;
		}

		ImGui::End();
	}

	void SpriteEditor::RenderFrameViewport()
	{
		if (ImGui::Begin("Preview"))
		{
			if (s_IsOpenedFile)
			{
				VegaUI::InputText(s_ClipName, "Clip Name");
				std::string path = s_CurrentPath;
				if (VegaUI::OpenTextureFile(FRAMEWORK.GetWindow().GetHandle(), path))
				{
					if (path != s_CurrentPath)
					{
						s_CurrentPath = path;
						SpriteEditor::SaveAnimationClip(s_OpenFilePath);
						SpriteEditor::OpenAniamtionClip(s_OpenFilePath);
						SpriteEditor::SetTarget(path);
					}
				}

				const char* loopTypes[] = { "Single", "Loop", "PingPong", "None" };
				const char* currLoopTypeString = loopTypes[(int)s_LoopType];
				if (ImGui::BeginCombo("Loop Type", currLoopTypeString))
				{
					for (int i = 0; i < 3; ++i)
					{
						bool isSelected = currLoopTypeString == loopTypes[i];
						if (ImGui::Selectable(loopTypes[i], isSelected))
						{
							currLoopTypeString = loopTypes[i];
							s_LoopType = (AnimationLoopTypes)i;
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (s_IsClickedFrame >= 0 && s_IsClickedFrame < s_Frames.size() && !s_Frames.empty())
				{
					sf::IntRect texRect = s_Frames[s_IsClickedFrame].getTextureRect();
					if (VegaUI::DrawControl4("Texture Rect", texRect))
					{
						s_Frames[s_IsClickedFrame].setTextureRect(texRect);
					}
					sf::Vector2f border = s_Frames[s_IsClickedFrame].getPosition();
					if (VegaUI::DrawControl2("Border", border))
					{
						s_Frames[s_IsClickedFrame].setPosition(border);
					}
					sf::Color color = s_Frames[s_IsClickedFrame].getColor();
					if (VegaUI::ColorEdit4(color, "Color"))
					{
						s_Frames[s_IsClickedFrame].setColor(color);
					}
					sf::Vector2f translate = s_Translate;
					if (VegaUI::DrawControl2("Translate", translate))
					{
						s_Translate = translate;
					}
					float rotation = s_Rotation;
					if (VegaUI::DrawControl1("Rotation", "Reset", rotation))
					{
						s_Rotation = rotation;
					}
					sf::Vector2f scale = s_Scale;
					if (VegaUI::DrawControl2("Scale", scale))
					{
						s_Scale = scale;
					}

					auto& buffer = s_PreviewBuffer->GetBuffer();
					buffer.clear();
					buffer.draw(s_Frames[s_IsClickedFrame]);
					buffer.display();
					ImGui::Image(buffer);
				}
			}
		}
		ImGui::End();
	}

	void SpriteEditor::SaveAnimationClip(const std::string& path)
	{
		if (s_Frames.empty())
			return;
		Database::LoadFromJson(path);
		auto& json = Database::GetJsonObject(path);
		json.clear();

		json["AnimationClip"]["FrameCount"] = s_Frames.size();
		json["AnimationClip"]["TexturePath"] = s_CurrentPath;
		json["AnimationClip"]["ClipName"] = s_ClipName;
		json["AnimationClip"]["LoopType"] = ToString(s_LoopType);
		auto& target = json["AnimationClip"];
		target["Transform"]["Translate"] = { s_Translate.x, s_Translate.y };
		target["Transform"]["Rotation"] = s_Rotation;
		target["Transform"]["Scale"] = { s_Scale.x, s_Scale.y };
		int size = static_cast<int>(s_Frames.size());
		for (int i = 0; i < size; ++i)
		{
			sf::Vector2f border = s_Frames[i].getPosition();
			sf::IntRect rect = s_Frames[i].getTextureRect();
			sf::Color color = s_Frames[i].getColor();

			std::string frameIndex = std::to_string(i);
			target[frameIndex];
			target[frameIndex]["Border"] = { border.x, border.y };
			target[frameIndex]["Rect"] = { rect.left, rect.top, rect.width, rect.height };
			target[frameIndex]["Color"] = { color.r, color.g, color.b, color.a };
		}
		Database::Unload(path);
	}

	void SpriteEditor::OpenAniamtionClip(const std::string& path)
	{
		std::string newPath = path;
		SpriteEditor::Clear();
		s_IsOpenedFile = true;
		s_OpenFilePath = newPath;
		SpriteEditor::SetActive(true);

		Database::LoadFromJson(s_OpenFilePath);
		auto& json = Database::GetJsonObject(s_OpenFilePath);

		unsigned int frameCount = json["AnimationClip"]["FrameCount"];
		s_CurrentPath = json["AnimationClip"]["TexturePath"];
		s_ClipName = json["AnimationClip"]["ClipName"];
		s_LoopType = ToLoopType(json["AnimationClip"]["LoopType"]);

		SpriteEditor::SetTarget(s_CurrentPath);

		auto& target = json["AnimationClip"];
		auto& j_translate = target["Transform"]["Translate"];
		auto& j_rotation = target["Transform"]["Rotation"];
		auto& j_scale = target["Transform"]["Scale"];

		s_Translate = { j_translate[0], j_translate[1] };
		s_Rotation = j_rotation;
		s_Scale = { j_scale[0], j_scale[1] };
		for (int i = 0; i < frameCount; ++i)
		{
			std::string frameIndex = std::to_string(i);
			auto& j_border = target[frameIndex]["Border"];
			auto& j_rect = target[frameIndex]["Rect"];
			auto& j_color = target[frameIndex]["Color"];

			s_Frames.push_back(s_WorkSprite);
			auto iter = (s_Frames.end() - 1);
			(*iter).setPosition(j_border[0], j_border[1]);
			(*iter).setTextureRect({ j_rect[0], j_rect[1], j_rect[2], j_rect[3] });
			(*iter).setColor({ j_color[0], j_color[1], j_color[2], j_color[3] });
		}
		Database::Unload(s_OpenFilePath);
	}

	void SpriteEditor::SetContext(Shared<Scene>& scene)
	{
		s_Scene = scene;
		FramebufferSpec spec;
		spec.Width = 200;
		spec.Height = 200;
		spec.MultisampleLevel = 4;
		s_FrameBuffer = Framebuffer::Create(spec);
		s_PreviewBuffer = Framebuffer::Create(spec);
		TEXTURE_MGR.Load("editor/icons/cursor.png");
		TEXTURE_MGR.Load("editor/icons/crop.png");
		s_MoveTex.setTexture(TEXTURE_MGR.Get("editor/icons/cursor.png"));
		s_CropTex.setTexture(TEXTURE_MGR.Get("editor/icons/crop.png"));
		s_ButtonSize = { 30.f, 30.f };
		s_BaseSize = s_EditorCamera.GetOrthoCamera().GetSize(); // OrthoCamera 기본 크기
	}

	bool SpriteEditor::SetTarget(const std::string& texturePath)
	{
		bool result = false;

		result = TEXTURE_MGR.Load(texturePath);
		auto& texture = TEXTURE_MGR.Get(texturePath);
		sf::Vector2u size = texture.getSize();
		s_CurrentPath = texturePath;
		
		s_WorkSprite = sf::Sprite();
		s_WorkSprite.setTexture(texture);
		s_FrameBuffer->Resize((float)size.x, (float)size.y);
		s_EditorCamera.SetViewport(0.0f, 0.0f, size.x * 2.0f, size.y * 2.0f);

		return result;
	}

	void SpriteEditor::SetActive(bool enabled)
	{
		s_IsActive = enabled;
		if (!enabled)
		{
			s_CurrentPath = "";
			s_ClipName = "None";
			s_LoopType = AnimationLoopTypes::Loop;
			s_Frames.clear();
			s_WorkSprite = sf::Sprite();
			s_Mode = SelectMode::Select;
			s_Translate = { 0.0f, 0.0f };
			s_Scale = { 1.0f, 1.0f };
			s_Rotation = 0.0f;
			s_IsOpenedFile = false;
			s_FrameBuffer->Clear();
		}
	}

	void SpriteEditor::OnUI()
	{
		if (!s_IsActive)
			return;

		s_FrameBuffer->Clear();
		s_FrameBuffer->GetBuffer().setView(s_EditorCamera.GetOrthoCamera());
		if (ImGui::Begin("Viewport", 0))
		{
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsWindowFocused())
				s_EditorCamera.SetActivated(true);
			else
				s_EditorCamera.SetActivated(false);

			// Render
			SpriteEditor::RenderSprite(s_WorkSprite, s_FrameBuffer);
			// Buttons
			SpriteEditor::RenderWithSelectMode(s_Mode);
			// Update Select Mode
			SpriteEditor::UpdateWithSelectMode();
		}
		// Update Frames
		SpriteEditor::UpdateFrames();
		SpriteEditor::RenderFrameViewport();
		ImGui::End();
	}

	void SpriteEditor::OnEvent(fz::Event& ev)
	{
		if (!s_IsActive)
			return;

		s_EditorCamera.OnEvent(ev);
	}

	void SpriteEditor::OnUpdate(float dt)
	{
		if (!s_IsActive)
			return;

		s_EditorCamera.OnUpdate(dt);
		ClampCameraBounds();
		ClampCameraZoom();
	}

	void SpriteEditor::Clear()
	{
		s_CurrentPath = "";
		s_ClipName = "None";
		s_LoopType = AnimationLoopTypes::Loop;
		s_Frames.clear();
		s_WorkSprite = sf::Sprite();
		s_Mode = SelectMode::Select;
		s_Translate = { 0.0f, 0.0f };
		s_Scale = { 1.0f, 1.0f };
		s_Rotation = 0.0f;
		s_OpenFilePath = "";
	}

	void SpriteEditor::RenderSprite(const sf::Sprite& sprite, Shared<fz::Framebuffer>& buffer)
	{
		sf::RenderStates state;
		state.texture = sprite.getTexture();
		buffer->GetBuffer().draw(s_WorkSprite, state);
		buffer->GetBuffer().draw(s_DrawRect);
		buffer->GetBuffer().display();
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		s_CurrentDrawSize = { windowSize.x - s_ButtonSize.x - 15.f, windowSize.y };
		ImGui::Image(s_FrameBuffer->GetBuffer(), s_CurrentDrawSize);
	}

	bool SpriteEditor::RenderWithSelectMode(SelectMode& dst)
	{
		bool result = false;
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		ImGui::SameLine(windowSize.x - s_ButtonSize.x);
		ImGui::BeginGroup();
		if (ImGui::ImageButton("SelectTex", s_MoveTex, s_ButtonSize))
		{
			dst = SelectMode::Movement;
			result = true;
		}
		ImGui::Spacing();
		if (ImGui::ImageButton("CropTex", s_CropTex, s_ButtonSize))
		{
			dst = SelectMode::Select;
			result = true;
		}
		ImGui::EndGroup();
		return result;
	}

	void SpriteEditor::UpdateWithSelectMode()
	{
		switch (s_Mode)
		{
			case SelectMode::Movement:
				s_EditorCamera.ActiveMouseClickMove(true);
				break;
			case SelectMode::Select:
				if (VegaUI::DragRect(start, end))
				{
					if (start.x != (start.x <= -1.0f))
					{
						DrawRect(true);
						s_HasCrop = true; 
					}
				}
				s_EditorCamera.ActiveMouseClickMove(false);
				break;
		}
	}

	void SpriteEditor::DrawRect(bool enabled)
	{
		ImVec2 winPos = ImGui::GetWindowPos();
		float titleBarH = ImGui::GetFrameHeight();
		ImVec2 currPos = ImGui::GetMousePos();
		ImVec2 windowSize = ImGui::GetWindowSize();
		const auto& texSize = s_WorkSprite.getTexture()->getSize();
		if (enabled)
		{
			float sw = texSize.x / (s_CurrentDrawSize.x);
			float ew = texSize.y / (s_CurrentDrawSize.y);

			FZLOG_DEBUG("sw = {0}, ew = {1}", sw, ew);
			sf::Vector2f adjustedStart = start;
			adjustedStart.x -= 7.f;
			adjustedStart.y -= 55.f;
			adjustedStart.x *= sw;
			adjustedStart.y *= ew;

			sf::Vector2f adjustedEnd = end;
			adjustedEnd.x -= 10.f;  
			adjustedEnd.y -= 57.f;  
			adjustedEnd.x *= sw;  
			adjustedEnd.y *= ew;  

			sf::Vector2f worldStartPos = s_FrameBuffer->GetBuffer().mapPixelToCoords({ (int)adjustedStart.x,(int)adjustedStart.y });
			sf::Vector2f worldEndPos = s_FrameBuffer->GetBuffer().mapPixelToCoords({ (int)adjustedEnd.x,(int)adjustedEnd.y });

			sf::Vector2f rectPos = { worldStartPos.x, worldStartPos.y  };
			sf::Vector2f rectSize = { worldEndPos.x - worldStartPos.x, worldEndPos.y - worldStartPos.y };

			sf::RectangleShape shape(rectSize);
			shape.setPosition(rectPos);
			shape.setOutlineColor({ 0, 130, 216, 255 }); 
			shape.setOutlineThickness(1.0f);
			shape.setFillColor(sf::Color::Transparent);

			s_DrawRect = shape;
			FZLOG_DEBUG("Rect Pos {0}, {1}", s_DrawRect.getPosition().x, s_DrawRect.getPosition().y);
			FZLOG_DEBUG("Rect Size {0}, {1}", s_DrawRect.getSize().x, s_DrawRect.getSize().y);
		}
		else
		{
			s_DrawRect = sf::RectangleShape();
		}
	}

} // namespace fz
