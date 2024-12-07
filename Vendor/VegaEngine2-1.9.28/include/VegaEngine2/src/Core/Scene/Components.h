#pragma once
#include <SFML/Graphics.hpp>
#include <Renderer/Camera.h>
#include "VegaScript.h"
#include <functional>

namespace fz {

	struct PrefabInstance
	{
		fz::Entity PrefabInstanceEntity;

		PrefabInstance() = default;
		PrefabInstance(const PrefabInstance&) = default;
		PrefabInstance(const fz::Entity& other)
			: PrefabInstanceEntity(other)
		{/*Empty*/}
	};

	struct TextComponent
	{
		int SortingOrder = 0;
		std::string FontPath;
		sf::Text Text;

		TextComponent() = default;
		TextComponent(const TextComponent&) = default;
		TextComponent(const sf::Text& other)
			: Text(other)
		{/*Empty*/}
	};

	struct RootEntityComponent
	{
		fz::Entity RootEntity;

		RootEntityComponent() = default;
		RootEntityComponent(const RootEntityComponent&) = default;
		RootEntityComponent(const fz::Entity& other)
			: RootEntity(other)
		{/*Empty*/}
	};

	struct ParentEntityComponent
	{
		fz::Entity ParentEntity;

		ParentEntityComponent() = default;
		ParentEntityComponent(const ParentEntityComponent&) = default;
		ParentEntityComponent(const fz::Entity& other)
			: ParentEntity(other)
		{/*Empty*/}
	};

	struct ChildEntityComponent
	{
		fz::Entity ParentEntity;
		std::vector<fz::Entity> CurrentChildEntities;

		ChildEntityComponent() = default;
		ChildEntityComponent(const ChildEntityComponent&) = default;
		ChildEntityComponent(const fz::Entity& other)
			: ParentEntity(other)
		{/*Empty*/}
	};

	struct TagComponent
	{
		bool Active = true;
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& other)
			: Tag(other)
		{/*Empty*/}
	};

	struct TransformComponent
	{
		fz::Transform Transform;
		fz::Transform AnimTransform;
		sf::Transform RenderTransform;
		bool IsChildRenderMode = false;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const fz::Transform& other)
			: Transform(other)
		{/*Empty*/}

		operator fz::Transform& () { return Transform; }
		operator const fz::Transform& () const { return Transform; }
	};

	struct SpriteComponent
	{
		bool Active = true;
		int SortingOrder = 0;
		fz::Sprite Sprite;

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(const fz::Sprite& other)
			: Sprite(other)
		{/*Empty*/}

		operator sf::Sprite& () { return Sprite; }
		operator const sf::Sprite& () const { return Sprite; }
	};

	struct CameraComponent
	{
		bool Active = true;
		OrthoCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false; //고정 종횡비(Fixed Aspect Ratio)
		sf::Vector2f Center = { 0.0f, 0.0f };

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const OrthoCamera& camera)
			: Camera(camera)
		{/*Empty*/}
		CameraComponent(const sf::Vector2f& center, const sf::Vector2f& size)
			: Camera(center, size)
		{/*Empty*/}

		inline operator OrthoCamera& ()				{ return Camera; }
		inline operator const OrthoCamera& () const	{ return Camera; }
	};

	struct RigidbodyComponent
	{
	protected:
		friend fz::Scene;
		friend fz::Entity;
		void* RuntimeBody = nullptr;

	public:
		enum class BodyType {
			Static, Dynamic, Kinematic
		};
		BodyType RigidType = BodyType::Static;
		bool FixedRotation = false;
		int GroupIndex = 0; // 0 초과일 때 같은 그룹끼리 충돌 x

		void AddForce(const sf::Vector2f& force);
		void AddPosition(const sf::Vector2f& pos);
		void AddPositionNoGravity(const sf::Vector2f& pos);
		void SetPosition(const sf::Vector2f& pos);
		void SetPosition(const sf::Vector2f& pos, float angle);
		void SetGravityScale(float scale);
		void SetLinearVelocity(const sf::Vector2f& velocity);
		sf::Vector2f GetLinearVelocity() const;

		bool IsOnGround(float rayLen = 0.45f);
		bool IsOnGround(float rayLen, sf::Vector2f& normal);
		bool IsOnGround(float rayLen, sf::Vector2f& normal, sf::Vector2f& pos);
		bool IsOnGround(float rayLen, sf::Vector2f& normal, sf::Vector2f& pos, float& fraction);

		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent& other) = default;
	}; 

	struct BoxCollider2DComponent
	{
	protected:
		friend fz::Scene;
		friend fz::HierarchyPanel;
		friend fz::EntitySerializer;

		void* RuntimeFixture = nullptr;
		bool IsTrigger = false;

		sf::Vector2f Offset = { 0.0f, 0.0f };
		sf::Vector2f Size = { 0.5f, 0.5f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

	public:
		void SetTrigger(bool enabled);
		sf::Vector2f GetSize() const { return Size * 2.0f; }
		sf::Vector2f GetHalfSize() const { return Size; }

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent& other) = default;
	};

	struct EdgeCollider2DComponent
	{
	protected:
		friend fz::Scene;
		friend fz::HierarchyPanel;
		friend fz::EntitySerializer;

		void* RuntimeFixture = nullptr;
		bool IsTrigger = false;
		bool IsOneSides = false;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		sf::Vector2f StartPos = { 0.0f, 0.0f };
		sf::Vector2f EndPos = { 1.0f, 0.0f };

	public:
		void SetTrigger(bool enabled);
		sf::Vector2f GetStartPos() const { return StartPos; }
		sf::Vector2f GetEndPos() const { return EndPos; }

		EdgeCollider2DComponent() = default;
		EdgeCollider2DComponent(const EdgeCollider2DComponent& other) = default;
	};

	struct NativeScriptComponent
	{
	public:
		Shared<VegaScript> Instance = nullptr;

		std::function<Shared<VegaScript>()> CreateInstanceFunc;

		std::function<void(Weak<VegaScript>)> OnCreateFunction;
		std::function<void(Weak<VegaScript>)> OnDestroyFunction;
		std::function<void(Weak<VegaScript>, float)> OnUpdateFunction;

		template <typename T>
		void Bind() {
			CreateInstanceFunc = []() { return static_cast<Shared<VegaScript>>(CreateShared<T>()); };

			OnCreateFunction = [](Weak<VegaScript> instance) { instance->Start(); };
			OnDestroyFunction = [](Weak<VegaScript> instance) { instance->OnDestroy(); };
			OnUpdateFunction = [](Weak<VegaScript> instance, float dt) { instance->OnUpdate(dt); };
		}
	};
} // namespace fz