#include "HierarchyPanel.h"
#include "../Editor2D.h"

using namespace std;

namespace fz {

	HierarchyPanel::HierarchyPanel(const Shared<Scene>& scene, EditorState* state)
		: m_Context(nullptr)
		, m_OnEntityRemove(false)
		, m_EditState(nullptr)
		, m_Active(true)
	{
		SetContext(scene, state);
	}

	void HierarchyPanel::SetContext(const Shared<Scene>& scene, EditorState* state)
	{
		m_Context = scene;
		m_SelectionContext = {};
		m_EditState = state;
	}

	void HierarchyPanel::SetActive(bool enabled)
	{
		m_Active = enabled;
	}

	void HierarchyPanel::OnImGuiRender()
	{
		if (!m_Active)
			return;

		if (ImGui::Begin("Hierarchy"))
		{
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered(0))
			{
				m_SelectionContext = {};
			}
			
			if (ImGui::BeginPopupContextWindow(0, 1))
			{
				if (*m_EditState == EditorState::Edit && ImGui::MenuItem("New Entity..."))
				{
					if (!m_Context)
						m_Context = CreateShared<Scene>(FRAMEWORK.GetWidth(), FRAMEWORK.GetHeight());
					fz::Entity entity = m_Context->CreateEntity("NewEntity");
					auto& rootComp = entity.AddComponent<RootEntityComponent>();
					rootComp.RootEntity = entity;
				}
				if (*m_EditState == EditorState::Edit && ImGui::MenuItem("Load Prefab"))
				{
					if (!m_Context)
					{
						m_Context = CreateShared<Scene>(FRAMEWORK.GetWidth(), FRAMEWORK.GetHeight());
					}
					auto nativeWindow = (sf::RenderWindow*)System::GetSystem().GetWindow().GetNativeWindow();
					HWND handle = (HWND)nativeWindow->getSystemHandle();
					std::string prefabOpenPath = VegaUI::OpenFile(handle, "Prefab File (*.prefab)\0*.prefab\0");
					if (!prefabOpenPath.empty())
						m_Context->LoadPrefab(prefabOpenPath);
				}
				ImGui::EndPopup();
			}

			int isClickNode = 0;
			if (m_Context)
			{
				auto RootView = m_Context->m_Registry.view<RootEntityComponent>();
				for (auto& rootEntity : RootView)
				{
					auto& tagComp = m_Context->m_Registry.get<TagComponent>(rootEntity);
					fz::Entity entity = { rootEntity, m_Context };
					isClickNode = this->DrawTreeNode(entity, tagComp.Tag.c_str());
				}
				if (isClickNode == 2)
				{
					ImGui::OpenPopup("##OpenPopup");
					if (ImGui::BeginPopup("##Popup"))
					{
						if (*m_EditState == EditorState::Edit && ImGui::MenuItem("Add Child Entity"))
						{
							Entity childEntity = m_SelectionContext.CreateChildEntity(Random.GetUUID(), "New Child Entity");
						}
						if (*m_EditState == EditorState::Edit && ImGui::MenuItem("Remove"))
						{
							m_OnEntityRemove = true;
						}
						ImGui::EndPopup();
					}
				}
			}

			if (ImGui::Begin("Properties"))
			{
				if (m_SelectionContext)
					this->DrawSceneComponents(m_SelectionContext);
			}
			ImGui::End();

			if (m_OnEntityRemove)
			{
				m_OnEntityRemove = false;
				m_Context->DeleteEntity(m_SelectionContext);
				m_SelectionContext = {};
			}
		}
		ImGui::End();
	}

	bool HierarchyPanel::DrawTreeNode(fz::Entity& entity, const char* tag)
	{
		if (entity.HasComponent<PrefabInstance>())
			return false;
		if (!m_Active)
			return false;

		auto nativeWindow = (sf::RenderWindow*)System::GetSystem().GetWindow().GetNativeWindow();
		HWND handle = (HWND)nativeWindow->getSystemHandle();
		ImGuiTreeNodeFlags flag = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

		bool result = false;
		if (ImGui::TreeNodeEx((const void*)(std::uint64_t)(std::uint32_t)entity, flag, tag))
		{
			if (entity.HasComponent<ChildEntityComponent>())
			{
				auto& childComp = entity.GetComponent<ChildEntityComponent>();
				for (auto& child : childComp.CurrentChildEntities)
				{
					auto& childTagComp = child.GetComponent<TagComponent>();
					result = DrawTreeNode(child, childTagComp.Tag.c_str());
				}
			}
			ImGui::TreePop();
		}

		if (!result && ImGui::IsItemClicked())

		{
			m_SelectionContext = entity;
			result = true;
		}

		if (!result && ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight))
		{
			if (*m_EditState == EditorState::Edit && ImGui::MenuItem("Add Child Entity"))
			{
				m_SelectionContext = entity;
				Entity childEntity = m_SelectionContext.CreateChildEntity(Random.GetUUID(), "New Child Entity");
			}
			if (*m_EditState == EditorState::Edit && ImGui::MenuItem("Remove"))
			{
				m_SelectionContext = entity;
				m_OnEntityRemove = true;
			}
			if (*m_EditState == EditorState::Edit && ImGui::MenuItem("Save Prefab"))
			{
				std::string prefabSavePath = VegaUI::SaveFile(handle, "Prefab File (*.prefab)\0*.prefab\0");
				if (!prefabSavePath.empty())
				{
					if (m_SelectionContext)
					{
						m_SelectionContext.SavePrefab(prefabSavePath);
					}
					else
					{
						FZLOG_WARN("Prefab 저장 실패! SelectContext가 존재하지 않습니다.");
					}
				}
			}
			ImGui::EndPopup();
			result = true;
		}
		
		return result;
	}

	void HierarchyPanel::DrawSceneComponents(fz::Entity& entity)
	{
		if (!m_Active)
			return;

		auto nativeWindow = (sf::RenderWindow*)System::GetSystem().GetWindow().GetNativeWindow();
		HWND handle = (HWND)nativeWindow->getSystemHandle();

		ImGuiTreeNodeFlags treeFlag = ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed;
		if (entity.HasComponent<TagComponent>())
		{
			auto& tagComp = entity.GetComponent<TagComponent>();
			if (VegaUI::Checkbox("Active", tagComp.Active))
			{
				entity.SetActiveWithChild(tagComp.Active);
			}
			VegaUI::InputText("Tag", tagComp.Tag);
		}

		{
			ImGui::SameLine();
			ImGui::PushItemWidth(-1);
			if (ImGui::Button("Add"))
			{
				if (*m_EditState == EditorState::Edit)
					ImGui::OpenPopup("AddComponent");
			}
			if (*m_EditState == EditorState::Edit && ImGui::BeginPopup("AddComponent"))
			{
				DisplayAddComponentEntry<CameraComponent>("Camera");
				DisplayAddComponentEntry<SpriteComponent>("Sprite");
				DisplayAddComponentEntry<TextComponent>("Text");
				DisplayAddComponentEntry<RigidbodyComponent>("Rigidbody");
				if (!m_SelectionContext.HasComponent<EdgeCollider2DComponent>())
					DisplayAddComponentEntry<BoxCollider2DComponent>("BoxCollider2D");
				if (!m_SelectionContext.HasComponent<BoxCollider2DComponent>())
					DisplayAddComponentEntry<EdgeCollider2DComponent>("EdgeCollider2D");
				ImGui::EndPopup();
			} 
		}

		if (entity.HasComponent<TransformComponent>())
		{
			auto& transform = entity.GetComponent<TransformComponent>().Transform;
			sf::Vector2f translate = transform.GetTranslate();
			float rotation = transform.GetRotation();
			sf::Vector2f scale = transform.GetScale();
			sf::Vector2f origin = transform.GetOrigin();

			if (ImGui::TreeNodeEx("TransformComponent1", treeFlag, "Translate"))
			{
				if (VegaUI::DrawControl2("Translate", translate, 1.0f, 1.0f))
					transform.SetTranslate(translate);
				if (VegaUI::DrawControl2("Scale", scale, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f))
					transform.SetScale(scale);
				if (VegaUI::DrawControl2("Origin", origin, 1.0f, 1.0f))
					transform.SetOrigin(origin);
				if (VegaUI::DrawControl1("Rotation", "Reset", rotation, 1.0f, 0.0f, 360.f, 0.0f))
					transform.SetRotation(rotation);
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<CameraComponent>())
		{
			if (ImGui::TreeNodeEx("CameraComponent", treeFlag, "Camera"))
			{
				bool isRemove = false;
				if (*m_EditState == EditorState::Edit)
				{
					isRemove = VegaUI::PopupContextItem("Remove CameraComponent", [&entity]() {
						entity.RemoveComponent<CameraComponent>(); });
				}

				if (!isRemove)
				{
					CameraComponent& cameraComp = entity.GetComponent<CameraComponent>();
					ImGui::Checkbox("Main", &cameraComp.Primary);
					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComp.FixedAspectRatio);

					sf::Vector2f center = cameraComp.Camera.GetCenter();
					if (VegaUI::DrawControl2("Center Position", center))
					{
						cameraComp.Camera.SetCenter(center);
					}
					float zoom = cameraComp.Camera.GetZoom();
					if (VegaUI::DrawControl1("Zoom", "Reset", zoom, 0.01f, 0.001f, 10.f, 1.0f))
					{
						cameraComp.Camera.Zoom(zoom);
					}

				}
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<SpriteComponent>())
		{
			if (ImGui::TreeNodeEx("SpriteComponent", treeFlag, "Sprite"))
			{
				bool isRemove = false;
				if (*m_EditState == EditorState::Edit)
				{
					isRemove = VegaUI::PopupContextItem("Remove SpriteComponent", [&entity]() {
						entity.RemoveComponent<SpriteComponent>();
															 });
				}
				if (!isRemove)
				{
					SpriteComponent& spriteComp = entity.GetComponent<SpriteComponent>();

					// SortingOrder
					int sortingOrder = spriteComp.SortingOrder;
					if (VegaUI::DrawControl1("Sorting Order", "Reset", sortingOrder, 1))
					{
						spriteComp.SortingOrder = sortingOrder;
					}

					std::string path = spriteComp.Sprite.GetTexturePath();
					if (VegaUI::OpenTextureFile(FRAMEWORK.GetWindow().GetHandle(), path))
					{
						if (path != spriteComp.Sprite.GetTexturePath())
						{
							TEXTURE_MGR.Load(path);
							spriteComp.Sprite.SetTexture(path);
						}
					}

					sf::IntRect texRect = spriteComp.Sprite.GetTextureRect();
					sf::Vector2u texSize = spriteComp.Sprite.GetSize();
					if (VegaUI::DrawControl4("Texture Rect", texRect,
						{ 1, 1, 1, 1 }, { 0, 0, 0, 0 }, 
						{ (int)texSize.x, (int)texSize.y, (int)texSize.x, (int)texSize.y }, 
						{ 0, 0, (int)texSize.x, (int)texSize.y }))
					{
						spriteComp.Sprite.SetTextureRect(texRect);
					}

					VegaUI::SelectOrigins(spriteComp.Sprite);

					sf::Color color = spriteComp.Sprite.GetColor();
					if (VegaUI::ColorEdit4(color, "Color"))
					{
						spriteComp.Sprite.SetColor(color);
					}

					bool maskMode = spriteComp.Sprite.IsMaskMode();
					sf::Color maskColor = spriteComp.Sprite.GetMaskColor();
					if (VegaUI::ColorEdit4WidthCheckbox(maskColor, maskMode, "Mask"))
					{
						spriteComp.Sprite.SetMaskColor(maskMode, maskColor);
					}
				}
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<TextComponent>())
		{
			if (ImGui::TreeNodeEx("TextComponent", treeFlag, "Text"))
			{
				bool isRemove = false;
				if (*m_EditState == EditorState::Edit)
				{
					isRemove = VegaUI::PopupContextItem("Remove TextComponent", [&entity]() {
						entity.RemoveComponent<TextComponent>(); });
				}

				if (!isRemove)
				{
					TextComponent& textComp = entity.GetComponent<TextComponent>();
					int sortingOrder = textComp.SortingOrder;
					if (VegaUI::DrawControl1("Sorting Order", "Reset", sortingOrder, 1))
					{
						textComp.SortingOrder = sortingOrder;
					}
					if (VegaUI::OpenFontFile(FRAMEWORK.GetWindow().GetHandle(), textComp.FontPath))
					{
						FONT_MGR.Load(textComp.FontPath);
						textComp.Text.setFont(FONT_MGR.Get(textComp.FontPath));
					}
					std::string str = textComp.Text.getString();
					if (VegaUI::InputText("Input Text", str))
					{
						textComp.Text.setString(str);
					}
					int size = textComp.Text.getCharacterSize();
					if (VegaUI::DrawControl1("Character Size", "Reset", size, 1, 1, 0, 1))
					{
						size = Utils::Clamp(size, 1, size);
						textComp.Text.setCharacterSize(size);
					}
					sf::Color color = textComp.Text.getColor();
					if (VegaUI::ColorEdit4(color, "Font Color"))
					{
						textComp.Text.setColor(color);
					}
					sf::Color outlineColor = textComp.Text.getOutlineColor();
					if (VegaUI::ColorEdit4(outlineColor, "Outline Color"))
					{
						textComp.Text.setOutlineColor(outlineColor);
					}
					float outlineThickness = textComp.Text.getOutlineThickness();
					if (VegaUI::DrawControl1("Outline Thickness", "Reset", outlineThickness))
					{
						textComp.Text.setOutlineThickness(outlineThickness);
					}
					sf::Color fillColor = textComp.Text.getFillColor();
					if (VegaUI::ColorEdit4(fillColor, "Fill Color"))
					{
						textComp.Text.setFillColor(fillColor);
					}
					float letterSpacing = textComp.Text.getLetterSpacing();
					if (VegaUI::DrawControl1("letter Spacing", "Reset", letterSpacing))
					{
						textComp.Text.setLetterSpacing(letterSpacing);
					}
					float lineSpacing = textComp.Text.getLineSpacing();
					if (VegaUI::DrawControl1("Line Spacing", "Reset", lineSpacing))
					{
						textComp.Text.setLineSpacing(lineSpacing);
					}
					sf::Vector2f origin = textComp.Text.getOrigin();
					if (VegaUI::DrawControl2("Origin", origin))
					{
						textComp.Text.setOrigin(origin);
					}
					sf::Vector2f position = textComp.Text.getPosition();
					if (VegaUI::DrawControl2("Position", position))
					{
						textComp.Text.setPosition(position);
					}
					sf::Vector2f scale = textComp.Text.getScale();
					if (VegaUI::DrawControl2("Scale", scale))
					{
						textComp.Text.setScale(scale);
					}
					float rotation = textComp.Text.getRotation();
					if (VegaUI::DrawControl1("Rotation", "Reset", rotation))
					{
						textComp.Text.setRotation(rotation);
					}
				}
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<RigidbodyComponent>())
		{
			if (ImGui::TreeNodeEx("RigidbodyComponent", treeFlag, "Rigidbody"))
			{
				bool isRemove = false;
				if (*m_EditState == EditorState::Edit)
				{
					isRemove = VegaUI::PopupContextItem("Remove RigidbodyComponent", [&entity]() {
						entity.RemoveComponent<RigidbodyComponent>();
															 });
				}
				if (!isRemove)
				{
					auto& rigidComp = entity.GetComponent<RigidbodyComponent>();
					const char* bodyTypes[] = { "Static", "Dynamic", "Kinematic" };
					const char* currBodyTypeString = bodyTypes[(int)rigidComp.RigidType];
					if (ImGui::BeginCombo("Body Type", currBodyTypeString))
					{
						for (int i = 0; i < 2; ++i)
						{
							bool isSelected = currBodyTypeString == bodyTypes[i];
							if (ImGui::Selectable(bodyTypes[i], isSelected))
							{
								currBodyTypeString = bodyTypes[i];
								rigidComp.RigidType = (RigidbodyComponent::BodyType)i;
							}
							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					ImGui::Checkbox("Fixed Rotation", &rigidComp.FixedRotation);
					int index = rigidComp.GroupIndex;
					if (VegaUI::DrawControl1("Group Index", "Reset", index, 1, 0, 0, 0))
					{
						rigidComp.GroupIndex = index;
					}
				}
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			if (ImGui::TreeNodeEx("ColliderComponent", treeFlag, "Collider"))
			{
				bool isRemove = false;
				if (*m_EditState == EditorState::Edit)
				{
					VegaUI::PopupContextItem("Remove ColliderComponent", [&entity]() {
						entity.RemoveComponent<BoxCollider2DComponent>(); });
				}

				if (!isRemove)
				{
					if (entity.HasComponent<BoxCollider2DComponent>())
					{
						auto& colComp = entity.GetComponent<BoxCollider2DComponent>();
						VegaUI::Checkbox("IsTrigger", colComp.IsTrigger);
						VegaUI::DrawControl2("Offset", colComp.Offset);
						VegaUI::DrawControl2("Size", colComp.Size);
						VegaUI::DrawControl1("Density", "Reset", colComp.Density);
						VegaUI::DrawControl1("Friction", "Reset", colComp.Friction);
						VegaUI::DrawControl1("Restitution", "Reset", colComp.Restitution);
						VegaUI::DrawControl1("Threshold", "Reset", colComp.RestitutionThreshold);
					}
				}
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<EdgeCollider2DComponent>())
		{
			if (ImGui::TreeNodeEx("ColliderComponent", treeFlag, "Collider"))
			{
				bool isRemove = VegaUI::PopupContextItem("Remove ColliderComponent", [&entity]() {
					entity.RemoveComponent<EdgeCollider2DComponent>(); });

				if (!isRemove)
				{
					if (entity.HasComponent<EdgeCollider2DComponent>())
					{
						auto& colComp = entity.GetComponent<EdgeCollider2DComponent>();
						VegaUI::Checkbox("IsTrigger", colComp.IsTrigger);
						VegaUI::Checkbox("IsOneSides", colComp.IsOneSides);
						VegaUI::DrawControl2("Start Position", colComp.StartPos, 0.1f, 0.1f, 0.0f, colComp.EndPos.x - 1.0f, 0.0f, colComp.EndPos.y - 1.0f);
						VegaUI::DrawControl2("End Position", colComp.EndPos, 0.1f, 0.1f, colComp.StartPos.x + 1.0f, 0.0f, colComp.StartPos.y, 0.0f );
						VegaUI::DrawControl1("Density", "Reset", colComp.Density);
						VegaUI::DrawControl1("Friction", "Reset", colComp.Friction);
						VegaUI::DrawControl1("Restitution", "Reset", colComp.Restitution);
						VegaUI::DrawControl1("Threshold", "Reset", colComp.RestitutionThreshold);
					}
				}
				ImGui::TreePop();
			}
		}
	}

} // namespace fz