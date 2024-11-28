
namespace fz {

	template<typename T>
	void HierarchyPanel::DisplayAddComponentEntry(const std::string& entryName)
	{
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				if (std::is_same<T, SpriteComponent>().value)
				{
					SpriteComponent& spriteComp = m_SelectionContext.GetComponent<SpriteComponent>();
					spriteComp.Sprite.SetOrigins(Origins::MC);
				}
				else if (std::is_same<T, CameraComponent>().value)
				{
					TransformComponent& transformComp = m_SelectionContext.GetComponent<TransformComponent>();
					sf::Vector2f pos = { 0.0f, 0.0f };
					if (m_SelectionContext.HasComponent<SpriteComponent>())
					{
						sf::Sprite& sprite = m_SelectionContext.GetComponent<SpriteComponent>().Sprite;
						pos = transformComp.Transform * sprite.getPosition();
					}
					else
					{
						pos = transformComp.Transform.GetTranslate();
					}
					CameraComponent& cameraComp = m_SelectionContext.GetComponent<CameraComponent>();
					cameraComp.Camera.SetCenter(pos);
					cameraComp.Camera.SetSize(FRAMEWORK.GetWidth(), FRAMEWORK.GetHeight());
				}
				else if (std::is_same<T, BoxCollider2DComponent>().value)
				{
					if (m_SelectionContext.HasComponent<SpriteComponent>())
					{
						BoxCollider2DComponent& colComp = m_SelectionContext.GetComponent<BoxCollider2DComponent>();
						fz::Transform& transform = m_SelectionContext.GetComponent<TransformComponent>();
						sf::Sprite& sprite = m_SelectionContext.GetComponent<SpriteComponent>().Sprite;
						sf::FloatRect rect = sprite.getGlobalBounds();
						colComp.Size.x *= (transform.GetScale().x * (rect.width - rect.left));
						colComp.Size.y *= (transform.GetScale().y * (rect.height - rect.top));
					}
				}
				else if (std::is_same<T, EdgeCollider2DComponent>().value)
				{
					if (m_SelectionContext.HasComponent<SpriteComponent>())
					{
						EdgeCollider2DComponent& colComp = m_SelectionContext.GetComponent<EdgeCollider2DComponent>();
						fz::Transform& transform = m_SelectionContext.GetComponent<TransformComponent>();
						sf::Sprite& sprite = m_SelectionContext.GetComponent<SpriteComponent>().Sprite;
						sf::FloatRect rect = sprite.getGlobalBounds();
						colComp.StartPos = transform.GetRawTransform() * colComp.StartPos;
						colComp.EndPos = transform.GetRawTransform() * colComp.EndPos;
					}
				}
				ImGui::CloseCurrentPopup();
			}
		}
	}

} // namespace fz