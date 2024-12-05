#include "pch.h"
#include "EntitySerializer.h"
#include "Entity.h"

#define DESERIALIZE(to, from) { if (!(from).is_null()) to = (from); }

namespace fz {


	namespace {
		static std::string ToString(RigidbodyComponent::BodyType type)
		{
			switch (type)
			{
				case RigidbodyComponent::BodyType::Static:
					return "Static";
				case RigidbodyComponent::BodyType::Dynamic:
					return "Dynamic";
				case RigidbodyComponent::BodyType::Kinematic:
					return "Kinematic";
				default:
					FZLOG_ASSERT(false, "타입 변환을 할 수 없습니다. 알 수 없는 타입입니다.");
					break;
			}
			return std::string("Unknown");
		}
		static RigidbodyComponent::BodyType ToBodyType(const std::string& type)
		{
			if (type == "Static")
				return RigidbodyComponent::BodyType::Static;
			else if (type == "Dynamic")
				return RigidbodyComponent::BodyType::Dynamic;
			else if (type == "Kinematic")
				return RigidbodyComponent::BodyType::Kinematic;
			FZLOG_ASSERT(false, "타입 변환을 할 수 없습니다. 알 수 없는 타입입니다.");
			return RigidbodyComponent::BodyType::Static;
		}
	}


	EntitySerializer::EntitySerializer(const fz::Entity& entity)
		: m_Entity(entity)
		, m_EntityUUID(m_Entity.m_UUID)
	{
		// Empty
	}

	void EntitySerializer::Serialize(fz::json& json)
	{
		// Root Entity 직렬화
		if (m_Entity.HasComponent<RootEntityComponent>())
			json[m_EntityUUID]["RootEntity"] = true;
		else
			json[m_EntityUUID]["RootEntity"] = false;

		if (m_Entity.HasComponent<ChildEntityComponent>())
		{
			json[m_EntityUUID]["HasChild"] = true;
			ChildEntityComponent& childComp = m_Entity.GetComponent<ChildEntityComponent>();
			for (auto& child : childComp.CurrentChildEntities)
			{
				ChildSerialize(json[m_Entity.m_UUID], child);
			}
		}
		else
		{
			json[m_EntityUUID]["HasChild"] = false;
		}

		this->SerializeTag(json[m_EntityUUID]);
		this->SerializeTransform(json[m_EntityUUID]);
		this->SerializeCamera(json[m_EntityUUID]);
		this->SerializeSprite(json[m_EntityUUID]);
		this->SerializeText(json[m_EntityUUID]);
		this->SerializeCollider(json[m_EntityUUID]);
		this->SerializeRigidBody(json[m_EntityUUID]);
	}

	void EntitySerializer::Deserialize(fz::json& json)
	{
		bool isRootEntity = json[m_EntityUUID]["RootEntity"];
		bool hasChild = json[m_EntityUUID]["HasChild"];

		if (isRootEntity)
		{
			auto& rootEntityComp = m_Entity.AddComponent<RootEntityComponent>();
			rootEntityComp.RootEntity = m_Entity;
		}
		for (auto it = json[m_EntityUUID].begin(); it != json[m_EntityUUID].end(); ++it)
		{
			const std::string& component = it.key();
			if (component == "TagComponent")
				this->DeserializeTag(json[m_EntityUUID]);
			else if (component == "TransformComponent")
				this->DeserializeTransform(json[m_EntityUUID]);
			else if (component == "CameraComponent")
				this->DeserializeCamera(json[m_EntityUUID]);
			else if (component == "SpriteComponent")
				this->DeserializeSprite(json[m_EntityUUID]);
			else if (component == "TextComponent")
				this->DeserializeText(json[m_EntityUUID]);
			else if (component == "RigidbodyComponent")
				this->DeserializeRigidBody(json[m_EntityUUID]);
			else if (component == "BoxCollider2DComponent")
				this->DeserializeCollider(json[m_EntityUUID]);
			else if (component == "EdgeCollider2DComponent")
				this->DeserializeCollider(json[m_EntityUUID]);
			else if (component == "RootEntity")
				isRootEntity = json[m_EntityUUID]["RootEntity"];
			else if (component == "HasChild")
				hasChild = json[m_EntityUUID]["HasChild"];
			else if (hasChild)
			{
				fz::Entity entity = m_Entity.CreateChildEntity(component, "New Child Entity...");
				this->ChildDerialize(json[m_EntityUUID], entity);
			}
		}
	}

	void EntitySerializer::ChildSerialize(fz::json& json, fz::Entity& child)
	{
		EntitySerializer serializer(child);
		serializer.Serialize(json);
	}

	void EntitySerializer::ChildDerialize(fz::json& json, fz::Entity& child)
	{
		EntitySerializer serializer(child);
		serializer.Deserialize(json);
	}

	void EntitySerializer::SerializeTag(json& json)
	{
		if (m_Entity.HasComponent<TagComponent>())
		{
			const auto& tag = m_Entity.GetComponent<TagComponent>();
			json["TagComponent"]["Tag"] = tag.Tag;
			json["TagComponent"]["Active"] = tag.Active;
		}
	}

	void EntitySerializer::SerializeCamera(json& json)
	{
		if (m_Entity.HasComponent<CameraComponent>())
		{
			const auto& cameraComp = m_Entity.GetComponent<CameraComponent>();
			const auto& camera = m_Entity.GetComponent<CameraComponent>().Camera;
			json["CameraComponent"]["FixedAspectRatio"] = cameraComp.FixedAspectRatio;
			json["CameraComponent"]["Primary"] = cameraComp.Primary;
			json["CameraComponent"]["Center"] = { camera.GetCenter().x, camera.GetCenter().y };
			json["CameraComponent"]["Size"] = { camera.GetSize().x, camera.GetSize().y };
			json["CameraComponent"]["Rotation"] = camera.GetRotation();
			json["CameraComponent"]["Zoom"] = camera.GetZoom();
			json["CameraComponent"]["Viewport"]["Left"] = camera.GetViewport().left;
			json["CameraComponent"]["Viewport"]["Top"] = camera.GetViewport().top;
			json["CameraComponent"]["Viewport"]["Width"] = camera.GetViewport().width;
			json["CameraComponent"]["Viewport"]["Height"] = camera.GetViewport().height;
		}
	}

	void EntitySerializer::SerializeTransform(json& json)
	{
		if (m_Entity.HasComponent<TransformComponent>())
		{
			const auto& transform = m_Entity.GetComponent<TransformComponent>().Transform;
			const auto& translate = transform.GetTranslate();
			const auto& scale = transform.GetScale();
			const auto& rotation = transform.GetRotation();
			const auto& origin = transform.GetOrigin();
			json["TransformComponent"]["Translate"] = { translate.x, translate.y };
			json["TransformComponent"]["Rotation"] = rotation;
			json["TransformComponent"]["Scale"] = { scale.x, scale.y };
			json["TransformComponent"]["Origin"] = { origin.x, origin.y };
		}
	}

	void EntitySerializer::SerializeSprite(json& json)
	{
		if (m_Entity.HasComponent<SpriteComponent>())
		{
			auto& spriteComp = m_Entity.GetComponent<SpriteComponent>();
			auto& sprite = spriteComp.Sprite;
			const auto& texPath = sprite.GetTexturePath();
			sf::Sprite& rawSprite = sprite;
			const auto& texRect = sprite.GetTextureRect();
			const auto& color = rawSprite.getColor();
			const auto& maskColor = sprite.GetMaskColor();
			std::vector<float> pos = { rawSprite.getPosition().x, rawSprite.getPosition().y };
			std::vector<float> scale = { rawSprite.getScale().x, rawSprite.getScale().y };
			std::vector<float> origin = { rawSprite.getOrigin().x, rawSprite.getOrigin().y };

			json["SpriteComponent"]["TexPath"] = texPath;
			json["SpriteComponent"]["TexRect"]["Left"] = texRect.left;
			json["SpriteComponent"]["TexRect"]["Top"] = texRect.top;
			json["SpriteComponent"]["TexRect"]["Width"] = texRect.width;
			json["SpriteComponent"]["TexRect"]["Height"] = texRect.height;
			json["SpriteComponent"]["Color"]["R"] = color.r;
			json["SpriteComponent"]["Color"]["G"] = color.g;
			json["SpriteComponent"]["Color"]["B"] = color.b;
			json["SpriteComponent"]["Color"]["A"] = color.a;
			json["SpriteComponent"]["Position"] = pos;
			json["SpriteComponent"]["Scale"] = scale;
			json["SpriteComponent"]["Rotation"] = rawSprite.getRotation();
			json["SpriteComponent"]["Origin"] = origin;
			json["SpriteComponent"]["Origins"] = Converter::ToString(sprite.GetOrigins());
			json["SpriteComponent"]["MaskMode"] = sprite.IsMaskMode();
			json["SpriteComponent"]["MaskColor"]["R"] = maskColor.r;
			json["SpriteComponent"]["MaskColor"]["G"] = maskColor.g;
			json["SpriteComponent"]["MaskColor"]["B"] = maskColor.b;
			json["SpriteComponent"]["MaskColor"]["A"] = maskColor.a;
			json["SpriteComponent"]["SortingOrder"] = spriteComp.SortingOrder;
		}
	}

	void EntitySerializer::SerializeText(json& json)
	{
		if (m_Entity.HasComponent<TextComponent>())
		{
			const TextComponent& textComp = m_Entity.GetComponent<TextComponent>();
			const auto& text = textComp.Text;
			int sortingOrder = textComp.SortingOrder;
			std::string path = textComp.FontPath;
			std::string str = text.getString();
			int size = text.getCharacterSize();
			sf::Color color = text.getColor();
			sf::Color outlineColor = text.getOutlineColor();
			float outlineThickness = text.getOutlineThickness();
			sf::Color fillColor = text.getFillColor();
			float letterSpacing = text.getLetterSpacing();
			float lineSpacing = text.getLineSpacing();
			sf::Vector2f origin = text.getOrigin();
			sf::Vector2f position = text.getPosition();
			sf::Vector2f scale = text.getScale();
			float rotation = text.getRotation();
			json["TextComponent"]["SortingOrder"] = sortingOrder;
			json["TextComponent"]["FontPath"] = path;
			json["TextComponent"]["String"] = str;
			json["TextComponent"]["Color"] = { color.r, color.g, color.b, color.a };
			json["TextComponent"]["CharacterSize"] = size;
			json["TextComponent"]["OutlineColor"] = { outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a };
			json["TextComponent"]["OutlineThickness"] = outlineThickness;
			json["TextComponent"]["FillColor"] = { fillColor.r, fillColor.g, fillColor.b, fillColor.a };
			json["TextComponent"]["LetterSpacing"] = letterSpacing;
			json["TextComponent"]["LineSpacing"] = lineSpacing;
			json["TextComponent"]["Origin"] = { origin.x, origin.y };
			json["TextComponent"]["Position"] = { position.x, position.y };
			json["TextComponent"]["Scale"] = { scale.x, scale.y };
			json["TextComponent"]["Rotation"] = rotation;

		}
	}

	void EntitySerializer::SerializeRigidBody(json& json)
	{
		if (m_Entity.HasComponent<RigidbodyComponent>())
		{
			auto& bodyComp = m_Entity.GetComponent<RigidbodyComponent>();
			bool fixedRotation = bodyComp.FixedRotation;
			std::string rigidType = ToString(bodyComp.RigidType);
			int groupIndex = bodyComp.GroupIndex;
			json["RigidbodyComponent"]["FixedRotation"] = fixedRotation;
			json["RigidbodyComponent"]["RigidType"] = rigidType;
			json["RigidbodyComponent"]["GroupIndex"] = groupIndex;
		}
	}

	void EntitySerializer::SerializeCollider(json& json)
	{
		if (m_Entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& colliderComp = m_Entity.GetComponent<BoxCollider2DComponent>();
			sf::Vector2f offset = colliderComp.Offset;
			sf::Vector2f size = colliderComp.Size;
			float density = colliderComp.Density;
			float friction = colliderComp.Friction;
			float restitution = colliderComp.Restitution;
			float restitutionThreshold = colliderComp.RestitutionThreshold;
			bool isTrigger = colliderComp.IsTrigger;

			json["BoxCollider2DComponent"]["IsTrigger"] = isTrigger;
			json["BoxCollider2DComponent"]["Offset"]["X"] = offset.x;
			json["BoxCollider2DComponent"]["Offset"]["Y"] = offset.y;
			json["BoxCollider2DComponent"]["Size"]["X"] = size.x;
			json["BoxCollider2DComponent"]["Size"]["Y"] = size.y;
			json["BoxCollider2DComponent"]["Density"] = density;
			json["BoxCollider2DComponent"]["Friction"] = friction;
			json["BoxCollider2DComponent"]["Restitution"] = restitution;
			json["BoxCollider2DComponent"]["RestitutionThreshold"] = restitutionThreshold;
		}
		if (m_Entity.HasComponent<EdgeCollider2DComponent>())
		{
			auto& colliderComp = m_Entity.GetComponent<EdgeCollider2DComponent>();
			sf::Vector2f startPos = colliderComp.StartPos;
			sf::Vector2f endPos = colliderComp.EndPos;
			bool isOneSides = colliderComp.IsOneSides;
			float density = colliderComp.Density;
			float friction = colliderComp.Friction;
			float restitution = colliderComp.Restitution;
			float restitutionThreshold = colliderComp.RestitutionThreshold;
			bool isTrigger = colliderComp.IsTrigger;

			json["EdgeCollider2DComponent"]["IsTrigger"] = isTrigger;
			json["EdgeCollider2DComponent"]["IsOneSides"] = isOneSides;
			json["EdgeCollider2DComponent"]["StartPos"]["X"] = startPos.x;
			json["EdgeCollider2DComponent"]["StartPos"]["Y"] = startPos.y;
			json["EdgeCollider2DComponent"]["EndPos"]["X"] = endPos.x;
			json["EdgeCollider2DComponent"]["EndPos"]["Y"] = endPos.y;
			json["EdgeCollider2DComponent"]["Density"] = density;
			json["EdgeCollider2DComponent"]["Friction"] = friction;
			json["EdgeCollider2DComponent"]["Restitution"] = restitution;
			json["EdgeCollider2DComponent"]["RestitutionThreshold"] = restitutionThreshold;
		}
	}

	void EntitySerializer::DeserializeTag(json& json)
	{
		TagComponent& tagComp = FindComponent<TagComponent>();
		tagComp.Tag = json["TagComponent"]["Tag"];
		tagComp.Active = json["TagComponent"]["Active"];
	}

	void EntitySerializer::DeserializeCamera(json& json)
	{
		CameraComponent& cameraComp = FindComponent<CameraComponent>();
		cameraComp.FixedAspectRatio = json["CameraComponent"]["FixedAspectRatio"];
		cameraComp.Primary = json["CameraComponent"]["Primary"];

		std::vector<float> centorPos = json["CameraComponent"]["Center"];
		std::vector<float> size = json["CameraComponent"]["Size"];
		float rotation = json["CameraComponent"]["Rotation"];
		float zoom = json["CameraComponent"]["Zoom"];
		float left = json["CameraComponent"]["Viewport"]["Left"];
		float top = json["CameraComponent"]["Viewport"]["Top"];
		float width = json["CameraComponent"]["Viewport"]["Width"];
		float height = json["CameraComponent"]["Viewport"]["Height"];

		auto& camera = cameraComp.Camera;
		camera.SetCenter(centorPos[0], centorPos[1]);
		camera.SetSize(size[0], size[1]);
		camera.SetRotation(rotation);
		camera.Zoom(zoom);
		camera.SetViewport({ left, top, width, height });
	}

	void EntitySerializer::DeserializeTransform(json& json)
	{
		TransformComponent& transfomComp = FindComponent<TransformComponent>();
		auto& transform = transfomComp.Transform;

		std::vector<float> translate = json["TransformComponent"]["Translate"];
		std::vector<float> scale = json["TransformComponent"]["Scale"];
		float rotation = json["TransformComponent"]["Rotation"];
		std::vector<float> origin = json["TransformComponent"]["Origin"];

		transform.SetTranslate(translate[0], translate[1]);
		transform.SetScale(scale[0], scale[1]);
		transform.SetRotation(rotation);
		transform.SetOrigin(origin[0], origin[1]);
	}

	void EntitySerializer::DeserializeSprite(json& json)
	{
		SpriteComponent& spriteComp = FindComponent<SpriteComponent>();
		auto& sprite = spriteComp.Sprite;

		std::string texPath = json["SpriteComponent"]["TexPath"];
		sprite.SetTexture(texPath);

		sf::Sprite& rawSprite = sprite;

		// Get SortingOrder
		spriteComp.SortingOrder = json["SpriteComponent"]["SortingOrder"];

		// Get MaskColor
		bool maskMode = json["SpriteComponent"]["MaskMode"];
		sf::Uint8 mr = json["SpriteComponent"]["MaskColor"]["R"];
		sf::Uint8 mg = json["SpriteComponent"]["MaskColor"]["G"];
		sf::Uint8 mb = json["SpriteComponent"]["MaskColor"]["B"];
		sf::Uint8 ma = json["SpriteComponent"]["MaskColor"]["A"];
		sprite.SetMaskColor(maskMode, { mr, mg, mb, ma });

		sf::Uint8 r = json["SpriteComponent"]["Color"]["R"];
		sf::Uint8 g = json["SpriteComponent"]["Color"]["G"];
		sf::Uint8 b = json["SpriteComponent"]["Color"]["B"];
		sf::Uint8 a = json["SpriteComponent"]["Color"]["A"];
		rawSprite.setColor({ r, g, b, a });

		std::vector<float> pos = json["SpriteComponent"]["Position"];
		std::vector<float> scale = json["SpriteComponent"]["Scale"];
		float rotation = json["SpriteComponent"]["Rotation"];
		std::vector<float> origin = json["SpriteComponent"]["Origin"];
		std::string originsStr = json["SpriteComponent"]["Origins"];
		Origins origins = Converter::StringToOrigins(originsStr);

		int left = json["SpriteComponent"]["TexRect"]["Left"];
		int top = json["SpriteComponent"]["TexRect"]["Top"];
		int width = json["SpriteComponent"]["TexRect"]["Width"];
		int height = json["SpriteComponent"]["TexRect"]["Height"];
		sprite.SetTextureRect({ left, top, width, height });
		if (origins == Origins::Custom)
			rawSprite.setOrigin(origin[0], origin[1]);
		else
			sprite.SetOrigins(origins);
		rawSprite.setPosition(pos[0], pos[1]);
		rawSprite.setScale(scale[0], scale[1]);
		rawSprite.setRotation(rotation);
	}

	void EntitySerializer::DeserializeText(json& json)
	{
		TextComponent& textComp = FindComponent<TextComponent>();
		auto& text = textComp.Text;

		int order = json["TextComponent"]["SortingOrder"];
		std::string path = json["TextComponent"]["FontPath"];
		std::string str = json["TextComponent"]["String"];
		const auto& color = json["TextComponent"]["Color"];
		int size = json["TextComponent"]["CharacterSize"];
		const auto& outlineColor = json["TextComponent"]["OutlineColor"];
		float outlineThickness = json["TextComponent"]["OutlineThickness"];
		const auto& fillColor = json["TextComponent"]["FillColor"];
		float letterSpacing = json["TextComponent"]["LetterSpacing"];
		float lineSpacing = json["TextComponent"]["LineSpacing"];
		const auto& origin = json["TextComponent"]["Origin"];
		const auto& position = json["TextComponent"]["Position"];
		const auto& scale = json["TextComponent"]["Scale"];
		float rotation = json["TextComponent"]["Rotation"];
		textComp.SortingOrder = order;
		textComp.FontPath = path;
		FONT_MGR.Load(textComp.FontPath);
		text.setFont(FONT_MGR.Get(textComp.FontPath));
		text.setString(str);
		text.setCharacterSize(size);
		text.setColor({ color[0], color[1], color[2], color[3] });
		text.setOutlineColor({ outlineColor[0], outlineColor[1], outlineColor[2], outlineColor[3] });
		text.setOutlineThickness(outlineThickness);
		text.setFillColor({ fillColor[0], fillColor[1], fillColor[2], fillColor[3] });
		text.setLetterSpacing(letterSpacing);
		text.setLineSpacing(lineSpacing);
		text.setOrigin({ origin[0], origin[1] });
		text.setPosition({ position[0], position[1] });
		text.setScale({ scale[0], scale[1] });
		text.setRotation(rotation);
	}

	void EntitySerializer::DeserializeRigidBody(json& json)
	{
		RigidbodyComponent& rigidComp = FindComponent<RigidbodyComponent>();
		std::string rigidType = json["RigidbodyComponent"]["RigidType"];
		rigidComp.FixedRotation = json["RigidbodyComponent"]["FixedRotation"];
		rigidComp.RigidType = ToBodyType(rigidType);
		DESERIALIZE(rigidComp.GroupIndex, json["RigidbodyComponent"]["GroupIndex"]);
	}

	void EntitySerializer::DeserializeCollider(json& json)
	{
		if (json.contains("BoxCollider2DComponent"))
		{
			BoxCollider2DComponent& colliderComp = FindComponent<BoxCollider2DComponent>();
			colliderComp.IsTrigger = json["BoxCollider2DComponent"]["IsTrigger"];
			colliderComp.Density = json["BoxCollider2DComponent"]["Density"];
			colliderComp.Friction = json["BoxCollider2DComponent"]["Friction"];
			colliderComp.Restitution = json["BoxCollider2DComponent"]["Restitution"];
			colliderComp.RestitutionThreshold = json["BoxCollider2DComponent"]["RestitutionThreshold"];
			colliderComp.Offset.x = json["BoxCollider2DComponent"]["Offset"]["X"];
			colliderComp.Offset.y = json["BoxCollider2DComponent"]["Offset"]["Y"];
			colliderComp.Size.x = json["BoxCollider2DComponent"]["Size"]["X"];
			colliderComp.Size.y = json["BoxCollider2DComponent"]["Size"]["Y"];
		}
		if (json.contains("EdgeCollider2DComponent"))
		{
			EdgeCollider2DComponent& colliderComp = FindComponent<EdgeCollider2DComponent>();
			colliderComp.IsTrigger = json["EdgeCollider2DComponent"]["IsTrigger"];
			colliderComp.IsOneSides = json["EdgeCollider2DComponent"]["IsOneSides"];
			colliderComp.Density = json["EdgeCollider2DComponent"]["Density"];
			colliderComp.Friction = json["EdgeCollider2DComponent"]["Friction"];
			colliderComp.Restitution = json["EdgeCollider2DComponent"]["Restitution"];
			colliderComp.RestitutionThreshold = json["EdgeCollider2DComponent"]["RestitutionThreshold"];
			colliderComp.StartPos.x = json["EdgeCollider2DComponent"]["StartPos"]["X"];
			colliderComp.StartPos.y = json["EdgeCollider2DComponent"]["StartPos"]["Y"];
			colliderComp.EndPos.x = json["EdgeCollider2DComponent"]["EndPos"]["X"];
			colliderComp.EndPos.y = json["EdgeCollider2DComponent"]["EndPos"]["Y"];
		}
	}

} // namespace fz
