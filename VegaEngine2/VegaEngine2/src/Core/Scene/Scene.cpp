#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "VegaScript.h"
#include "EditorCamera.h"
#include "EntitySerializer.h"
#include "CollisionHandler.h"
#include "SFML/Graphics.hpp"

namespace fz {

	namespace {
		static b2BodyType ToBox2dBodyType(const fz::RigidbodyComponent::BodyType& rigidType)
		{
			switch (rigidType)
			{
				case RigidbodyComponent::BodyType::Static:
					return b2BodyType::b2_staticBody;
				case RigidbodyComponent::BodyType::Dynamic:
					return b2BodyType::b2_dynamicBody;
				case RigidbodyComponent::BodyType::Kinematic:
					return b2BodyType::b2_kinematicBody;
				default:
					FZLOG_ASSERT(false, "타입 변환을 할 수 없습니다. 알 수 없는 타입입니다.");
					break;
			}
			return b2BodyType();
		}
	}

	Scene::Scene(unsigned int width, unsigned int height, unsigned int mulltisampleLevel, const std::string& uuid)
		: m_UUID(uuid.empty() ? Random.GetUUID() : uuid)
		, m_IsDebugMode(false)
		, m_prefabTempPath("Prefabs/temp.prefab")
		, m_prefabInstanceCount(0)
		, m_SceneChanged(false)
	{
		FramebufferSpec frameSpec;
		frameSpec.Width = width;
		frameSpec.Height = height;
		frameSpec.MultisampleLevel = 8;
		m_FrameBuffer = Framebuffer::Create(frameSpec);
	}

	Scene::~Scene()
	{
		this->ReleaseNativeComponent();
	}

	void Scene::DestroyInstance(GameObject& prefabInstance)
	{
		if (!prefabInstance.HasComponent<PrefabInstance>())
		{
			FZLOG_WARN("현재 제거하려 하는 인스턴스가 Prefab의 인스턴스가 아닙니다.");
			return;
		}
		m_RemoveInstanceList.push_back(prefabInstance);
		prefabInstance = GameObject();
	}

	Entity Scene::CreateEntity(const std::string& tagName)
	{
		Entity entity = { m_Registry.create(), shared_from_this() };
		auto& tagComp = entity.AddComponent<TagComponent>(tagName);
		if (tagComp.Tag.empty())
			tagComp.Tag = "Entity";
		if (!tagName.empty())
		{
			tagComp.Tag = tagName;
		}
		entity.AddComponent<TransformComponent>();
		m_EntityPool.insert({ entity.m_UUID, entity.m_Handle });
		return entity;
	}

	Entity Scene::CreateEntityWithUUID(const std::string& tagName, const std::string& uuid)
	{
		Entity entity = { m_Registry.create(), shared_from_this() };
		entity.m_UUID = uuid;
		auto& tagComp = entity.AddComponent<TagComponent>(tagName);
		if (tagComp.Tag.empty())
			tagComp.Tag = "Entity";
		if (!tagName.empty())
		{
			tagComp.Tag = tagName;
		}
		entity.AddComponent<TransformComponent>();
		m_EntityPool.insert({ entity.m_UUID, entity.m_Handle });
		return entity;
	}

	void Scene::CopyEntityForPrefab(fz::Entity dst, fz::Entity src, bool isRootTransform)
	{
		if (!dst.HasComponent<PrefabInstance>())
		{
			auto& dstPrefabComp = dst.AddComponent<PrefabInstance>();
			dstPrefabComp.PrefabInstanceEntity = dst;
		}

		if (src.HasComponent<RootEntityComponent>())
		{
			auto& rootComp = dst.AddComponent<RootEntityComponent>();
			rootComp.RootEntity = dst;
			rootComp.RootEntity.m_Scene = this->shared_from_this();
		}
		else if (src.HasComponent<ChildEntityComponent>())
		{
			auto& dstChildComp = dst.AddComponent<ChildEntityComponent>();
			dstChildComp.ParentEntity = dst;
			auto& srcChildComp = src.GetComponent<ChildEntityComponent>();
			int srcChildCount = static_cast<int>(srcChildComp.CurrentChildEntities.size());
			for (int i = 0; i < srcChildCount; ++i)
			{
				auto& srcChild = srcChildComp.CurrentChildEntities[i];
				fz::Entity dstChild = CreateEntity();
				dstChildComp.CurrentChildEntities.push_back(dstChild);
				CopyEntityForPrefab(dstChild, srcChild);
			}
		}
		// Tag
		{
			auto& srcTagComp = src.GetComponent<TagComponent>();
			auto& dstTagComp = dst.GetComponent<TagComponent>();
			dstTagComp.Active = true; // TODO: prefab instance는 무조건 활성화
			dstTagComp.Tag += srcTagComp.Tag;
		}
		// Transform 
		{
			auto& srcTransformComp = src.GetComponent<TransformComponent>();
			auto& dstTransformComp = dst.GetComponent<TransformComponent>();
			if (!isRootTransform) // 지정된 Transform일 경우 복사 제외 (차일드 엔티티일 경우)
				dstTransformComp.Transform = srcTransformComp.Transform;
			dstTransformComp.AnimTransform = srcTransformComp.AnimTransform;
		}
		// etc
		{
			if (src.HasComponent<SpriteComponent>())
			{
				auto& srcSpriteComp = src.GetComponent<SpriteComponent>();
				auto& dstSpriteComp = dst.AddComponent<SpriteComponent>();
				dstSpriteComp.Active = srcSpriteComp.Active;
				dstSpriteComp.Sprite = srcSpriteComp.Sprite;
				dstSpriteComp.SortingOrder = srcSpriteComp.SortingOrder;
			}
			if (src.HasComponent<TextComponent>())
			{
				auto& srcTextComp = src.GetComponent<TextComponent>();
				auto& dstTextComp = dst.AddComponent<TextComponent>();
				dstTextComp.FontPath = srcTextComp.FontPath;
				FONT_MGR.Load(dstTextComp.FontPath);
				auto& srcText = srcTextComp.Text;
				auto& dstText = dstTextComp.Text;
				dstText.setFont(FONT_MGR.Get(dstTextComp.FontPath));
				dstText.setString(srcText.getString());
				dstText.setCharacterSize(srcText.getCharacterSize());
				dstText.setColor(srcText.getColor());
				dstText.setOutlineColor(srcText.getOutlineColor());
				dstText.setOutlineThickness(srcText.getOutlineThickness());
				dstText.setFillColor(srcText.getFillColor());
				dstText.setLetterSpacing(srcText.getLetterSpacing());
				dstText.setLineSpacing(srcText.getLineSpacing());
				dstText.setOrigin(srcText.getOrigin());
				dstText.setPosition(srcText.getPosition());
				dstText.setScale(srcText.getScale());
				dstText.setRotation(srcText.getRotation());
			}
			if (src.HasComponent<CameraComponent>())
			{
				auto& srcCameraComp = src.GetComponent<CameraComponent>();
				auto& dstCameraComp = dst.AddComponent<CameraComponent>();
				dstCameraComp.Active = srcCameraComp.Active;
				dstCameraComp.Camera = srcCameraComp.Camera;
				dstCameraComp.FixedAspectRatio = srcCameraComp.FixedAspectRatio;
				dstCameraComp.Primary = srcCameraComp.Primary;
			}
			if (src.HasComponent<RigidbodyComponent>())
			{
				auto& srcRigidbodyComp = src.GetComponent<RigidbodyComponent>();
				auto& dstRigidbodyComp = dst.AddComponent<RigidbodyComponent>();
				dstRigidbodyComp.FixedRotation = srcRigidbodyComp.FixedRotation;
				dstRigidbodyComp.RigidType = srcRigidbodyComp.RigidType;
				dstRigidbodyComp.GroupIndex = srcRigidbodyComp.GroupIndex;
			}
			if (src.HasComponent<BoxCollider2DComponent>())
			{
				auto& srcBoxComp = src.GetComponent<BoxCollider2DComponent>();
				auto& dstBoxComp = dst.AddComponent<BoxCollider2DComponent>();
				dstBoxComp.Density = srcBoxComp.Density;
				dstBoxComp.Friction = srcBoxComp.Friction;
				dstBoxComp.IsTrigger = srcBoxComp.IsTrigger;
				dstBoxComp.Offset = srcBoxComp.Offset;
				dstBoxComp.Restitution = srcBoxComp.Restitution;
				dstBoxComp.RestitutionThreshold = srcBoxComp.RestitutionThreshold;
				dstBoxComp.Size = srcBoxComp.Size;
			}
			else if (src.HasComponent<EdgeCollider2DComponent>())
			{
				auto& srcBoxComp = src.GetComponent<EdgeCollider2DComponent>();
				auto& dstBoxComp = dst.AddComponent<EdgeCollider2DComponent>();
				dstBoxComp.StartPos = srcBoxComp.StartPos;
				dstBoxComp.EndPos = srcBoxComp.EndPos;
				dstBoxComp.IsOneSides = srcBoxComp.IsOneSides;
				dstBoxComp.Density = srcBoxComp.Density;
				dstBoxComp.Friction = srcBoxComp.Friction;
				dstBoxComp.IsTrigger = srcBoxComp.IsTrigger;
				dstBoxComp.Restitution = srcBoxComp.Restitution;
				dstBoxComp.RestitutionThreshold = srcBoxComp.RestitutionThreshold;
			}
			if (src.HasComponent<NativeScriptComponent>())
			{
				auto& srcNativeComp = src.GetComponent<NativeScriptComponent>();
				auto& dstNativeComp = dst.AddComponent<NativeScriptComponent>();
				dstNativeComp.CreateInstanceFunc = srcNativeComp.CreateInstanceFunc;
				dstNativeComp.OnCreateFunction = srcNativeComp.OnCreateFunction;
				dstNativeComp.OnDestroyFunction = srcNativeComp.OnDestroyFunction;
				dstNativeComp.OnUpdateFunction = srcNativeComp.OnUpdateFunction;
			}
			m_LoadPrefabInstanceList.push_back(dst);
		}
	}

	void Scene::ReleaseNativeComponent()
	{
		auto nativeView = m_Registry.view<NativeScriptComponent>();
		nativeView.each([&](auto entity, NativeScriptComponent& nsc)
						{
							if (nsc.Instance)
								nsc.OnDestroyFunction(nsc.Instance);
						});
	}

	void Scene::DeleteEntity(fz::Entity& entity)
	{
		if (entity && entity.HasComponent<ChildEntityComponent>())
		{
			ChildEntityComponent& childComp = entity.GetComponent<ChildEntityComponent>();
			auto& childs = childComp.CurrentChildEntities;
			for (auto begin = childs.begin(); begin != childs.end();)
			{
				GameObject del = *begin;
				begin = childs.erase(begin);
				DeleteEntity(del);
			}
			childComp.CurrentChildEntities.clear();
		}
		if (Scene::s_World && entity.HasComponent<RigidbodyComponent>())
		{
			auto& rigidComp = entity.GetComponent<RigidbodyComponent>();
			b2Body* body = (b2Body*)rigidComp.RuntimeBody;
			if (body)
			{
				Scene::s_World->DestroyBody(body);
			}
		}
		if (entity.HasComponent<NativeScriptComponent>())
		{
			auto& nativeComp = entity.GetComponent<NativeScriptComponent>();
			nativeComp.OnDestroyFunction(nativeComp.Instance);
		}
		auto it = m_EntityPool.find(entity.m_UUID);
		if (it != m_EntityPool.end())
		{
			m_EntityPool.erase(it);
		}
		if (entity && entity.HasComponent<ParentEntityComponent>())
		{
			auto& parentComp = entity.GetComponent<ParentEntityComponent>();
			auto& childs = parentComp.ParentEntity.GetComponent<ChildEntityComponent>().CurrentChildEntities;
			for (auto begin = childs.begin(); begin != childs.end(); ++begin)
			{
				if (*begin == entity)
				{
					childs.erase(begin);
					break;
				}
			}
		}
		FZLOG_DEBUG("entity 삭제 handle = {0}, tag = {1}", (int)entity.m_Handle, entity.GetComponent<TagComponent>().Tag);
		m_Registry.destroy(entity.m_Handle);
	}

	void Scene::StartPhysics()
	{
		StopPhysics();

		s_World = new b2World({ 0.0f, 9.8f });

		m_CollistionHandler.SetContext(shared_from_this());
		s_World->SetContactListener(&m_CollistionHandler);

		auto view = m_Registry.view<RigidbodyComponent>();
		for (auto& handle : view)
		{
			fz::Entity entity = { handle, shared_from_this() };
			LoginPhysicsWorld(entity);

			// 보간 비활성화
			auto& rigidBody = entity.GetComponent<RigidbodyComponent>();
			b2Body* body = static_cast<b2Body*>(rigidBody.RuntimeBody);
			if (body)
			{
				body->SetSleepingAllowed(false); // 항상 위치 업데이트
			}
		}
	}

	void Scene::StopPhysics()
	{
		if (s_World)
		{
			delete s_World;
			s_World = nullptr;
		}
	}

	void Scene::LoginPhysicsWorld(fz::Entity entity)
	{
		if (!entity.HasComponent<RigidbodyComponent>())
			return;

		auto& transformComp = entity.GetComponent<TransformComponent>();
		auto& rigidBodyComp = entity.GetComponent<RigidbodyComponent>();
		auto& tagComp = entity.GetComponent<TagComponent>();
		const auto& transform = entity.GetWorldTransform();
		const b2Vec2& meterPos = Utils::PixelToMeter(entity.GetWorldPosition());
		b2Body* body = nullptr;
		b2BodyDef bodyDef;
		bodyDef.type = ToBox2dBodyType(rigidBodyComp.RigidType);
		bodyDef.position.Set(meterPos.x, meterPos.y);
		bodyDef.angle = Utils::DegreeToRadian(transformComp.Transform.GetRotation());
		body = s_World->CreateBody(&bodyDef);
		if (body)
		{ 
			body->SetFixedRotation(rigidBodyComp.FixedRotation);
			rigidBodyComp.RuntimeBody = body;
		}
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& collider = entity.GetComponent<BoxCollider2DComponent>();

			const b2Vec2& meterBoxSize = Utils::PixelToMeter({ std::abs(collider.Size.x * transformComp.Transform.GetScale().x),
															   std::abs(collider.Size.y * transformComp.Transform.GetScale().y) });
			if (meterBoxSize.x <= 0.0f || meterBoxSize.y <= 0.0f)
			{
				FZLOG_WARN("Collider 생성 실패, Box Size 크기를 잘못 설정하였습니다. {0}, {1},", meterBoxSize.x, meterBoxSize.y);
				return;
			}

			auto& tag = entity.GetComponent<TagComponent>();

			b2PolygonShape polygonShape;
			polygonShape.SetAsBox(meterBoxSize.x, meterBoxSize.y);

			b2FixtureDef fixtureDef;
			fixtureDef.isSensor = collider.IsTrigger;
			fixtureDef.shape = &polygonShape;
			fixtureDef.density = collider.Density;
			fixtureDef.friction = collider.Friction;
			fixtureDef.restitution = collider.Restitution;
			fixtureDef.restitutionThreshold = collider.RestitutionThreshold;
			fixtureDef.userData.pointer = static_cast<uintptr_t>(entity.m_Handle);
			fixtureDef.filter.groupIndex = rigidBodyComp.GroupIndex;
			collider.RuntimeFixture = body->CreateFixture(&fixtureDef);
		}
		else if (entity.HasComponent<EdgeCollider2DComponent>())
		{
			auto& collider = entity.GetComponent<EdgeCollider2DComponent>();

			const b2Vec2& newStart = Utils::PixelToMeter(collider.StartPos);
			const b2Vec2& newEnd = Utils::PixelToMeter(collider.EndPos);

			b2EdgeShape edgeShape;
			if (collider.IsOneSides)
			{
				b2Vec2 direction = newEnd - newStart;
				b2Vec2 normal(direction.y, -direction.x);
				float length = normal.Length(); 
				if (length != 0) {
					normal.x /= length;
					normal.y /= length;
				}
				b2Vec2 v0 = newStart + normal;
				b2Vec2 v3 = newEnd + normal;
				edgeShape.SetOneSided(v0, newStart, newEnd, v3);
			}
			else
			{
				edgeShape.SetTwoSided(newStart, newEnd);
			}

			b2FixtureDef fixtureDef;
			fixtureDef.isSensor = collider.IsTrigger;
			fixtureDef.shape = &edgeShape;
			fixtureDef.density = collider.Density;
			fixtureDef.friction = collider.Friction;
			fixtureDef.restitution = collider.Restitution;
			fixtureDef.restitutionThreshold = collider.RestitutionThreshold;
			fixtureDef.userData.pointer = static_cast<uintptr_t>(entity.m_Handle);
			fixtureDef.filter.groupIndex = rigidBodyComp.GroupIndex;
			collider.RuntimeFixture = body->CreateFixture(&fixtureDef);
		}
	}

	void Scene::OnUpdateEditor(float dt, EditorCamera& editorCamera)
	{
		editorCamera.OnUpdate(dt);
		this->OnRenderEditorSprite(&editorCamera.GetOrthoCamera());
	}

	void Scene::OnPreUpdate()
	{
		this->ReleasePrefabInstancies();
		this->StartPhysics();
		this->OnPreUpdateScript();
	}

	void Scene::OnUpdate(float dt)
	{
		OrthoCamera* camera = nullptr;
		sf::Transform transform = sf::Transform::Identity;
		this->OnCreateRuntimeInstance();
		this->OnDestroyRuntimeInstance();
		this->OnUpdateScript(dt);
		this->OnUpdateCamera(&camera, transform);
		this->OnUpdatePhysicsSystem(dt);
		this->OnRuntimeRenderDrawable(camera, transform);
	}

	void Scene::OnPostUpdate()
	{
		this->ReleasePostRemoveInstancies();
		this->ReleasePrefabInstancies();
		this->OnPostUpdateScript();
		this->StopPhysics();
	}

	void Scene::OnViewportResize(unsigned int width, unsigned int height)
	{
		if (width < 0 || height < 0)
			return;

		m_FrameBuffer->Resize(width, height);

		auto view = m_Registry.view<CameraComponent>();
		for (auto& entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
 				cameraComponent.Camera.SetSize(width, height);
			}
		}
	}

	void Scene::OnDestroyRuntimeInstance()
	{
		if (!s_World->IsLocked())
		{
			while (!m_RemoveInstanceList.empty())
			{
				GameObject delObj = m_RemoveInstanceList.front();
				m_RemoveInstanceList.pop_front();
				{ // PrefabInstance Pool에서 삭제
					auto it = m_PrefabInstancePool.find(delObj.m_UUID);
					if (it != m_PrefabInstancePool.end())
					{
						m_PrefabInstancePool.erase(it);
					}
				}
				delObj.SetActiveWithChild(false);
				{ // Entity Pool에서 삭제
					auto it = m_EntityPool.find(delObj.m_UUID);
					if (it != m_EntityPool.end())
					{
						m_EntityPool.erase(it);
					}
				}
				delObj.DeleteRigidbodyWithChilds();
				m_PostRemoveInstanceList.push_back(delObj);
			}
		}
	}

	void Scene::ReleasePrefabInstancies()
	{
		for (auto& instance : m_PrefabInstancePool)
		{
			fz::Entity deleteEntity = { instance.first, instance.second, shared_from_this() };
			this->DeleteEntity(deleteEntity);
		}
		m_PrefabInstancePool.clear();
	}

	void Scene::ReleasePostRemoveInstancies()
	{
		for (auto& instance : m_PostRemoveInstanceList)
		{
			this->DeleteEntity(instance);
		}
		m_PrefabInstancePool.clear();
	}

	fz::Entity Scene::GetEntityFromUUID(const std::string& uuid)
	{
		auto it = m_EntityPool.find(uuid);
		std::string itUUID = "";
		entt::entity itHandle = entt::null;
		if (it != m_EntityPool.end())
		{
			itUUID = it->first;
			itHandle = it->second;
			
		}
		return fz::Entity(itUUID, itHandle, shared_from_this());
	}

	Entity Scene::GetEntityFromTag(const std::string& tag)
	{
		auto tagView = GetEntities<TagComponent>();
		for (auto& tags : tagView)
		{
			auto& tagComp = tagView.get<TagComponent>(tags);
			if (tagComp.Tag == tag)
				return { tags, shared_from_this() };
		}
		return {};
	}

	GameObject Scene::Instantiate(GameObject entity, const sf::Vector2f& position, float rotation)
	{
		std::string tempTag = ("prefabInstance" + std::to_string(m_prefabInstanceCount));
		fz::Transform transform;
		transform.SetTranslate(position);
		transform.SetRotation(rotation);
		return this->Instantiate(entity, tempTag, transform);
	}

	sf::Vector2f Scene::GetWorldMousePos() const
	{
		const auto& mousePos = Input::GetMousePosition();
		return this->PixelToCoords({ (int)mousePos.x, (int)mousePos.y });
	}

	sf::Vector2f Scene::PixelToCoords(const sf::Vector2i& pixelPos) const
	{
		auto& buffer = m_FrameBuffer->GetBuffer();
		return buffer.mapPixelToCoords(pixelPos);
	}

	sf::Vector2f Scene::PixelToCoords(const sf::Vector2i& pixelPos, const fz::OrthoCamera& camera) const
	{
		auto& buffer = m_FrameBuffer->GetBuffer();
		return buffer.mapPixelToCoords(pixelPos, camera);
	}

	GameObject Scene::Instantiate(const std::string& tag, const sf::Vector2f& position)
	{
		std::string tempTag = ("prefabInstance" + std::to_string(m_prefabInstanceCount));
		fz::Transform transform;
		transform.SetTranslate(position);
		GameObject prefab = this->GetEntityFromTag(tag);
		return this->Instantiate(prefab, tempTag, transform);
	}

	GameObject Scene::Instantiate(const std::string& tag, const sf::Vector2f& position, const sf::Vector2f& scale)
	{
		std::string tempTag = ("prefabInstance" + std::to_string(m_prefabInstanceCount));
		fz::Transform transform;
		transform.SetTranslate(position);
		transform.SetScale(scale);
		GameObject prefab = this->GetEntityFromTag(tag);
		return this->Instantiate(prefab, tempTag, transform);
	}

	GameObject Scene::Instantiate(GameObject entity, const sf::Vector2f& position)
	{
		std::string tempTag = ("prefabInstance" + std::to_string(m_prefabInstanceCount));
		fz::Transform transform;
		transform.SetTranslate(position);
		return this->Instantiate(entity, tempTag, transform);
	}

	GameObject Scene::Instantiate(GameObject entity, const fz::Transform& transform)
	{
		std::string tempTag = ("prefabInstance" + std::to_string(m_prefabInstanceCount));
		return this->Instantiate(entity, tempTag, transform);
	}

	GameObject Scene::Instantiate(GameObject entity, const std::string& tag, const fz::Transform& transform)
	{
		fz::Entity newEntity = CreateEntity(tag);
		auto& prefabComp = newEntity.AddComponent<PrefabInstance>();
		prefabComp.PrefabInstanceEntity = newEntity;

		auto& transformComp = newEntity.GetComponent<TransformComponent>();
		transformComp.Transform = transform;
		CopyEntityForPrefab(newEntity, entity, true);

		m_prefabInstanceCount++;
		return newEntity;
	}

	void Scene::OnPreUpdateScript()
	{
		auto nativeView = m_Registry.view<TagComponent, NativeScriptComponent>();
		nativeView.each([&](auto entity, TagComponent& tag, NativeScriptComponent& nsc)
						{
							if (nsc.Instance == nullptr)
							{
								nsc.Instance = nsc.CreateInstanceFunc();
								if (!nsc.Instance->m_Entity)
									nsc.Instance->m_Entity = { entity, shared_from_this() };
								nsc.OnCreateFunction(nsc.Instance);
							}
						});
	}

	void Scene::OnUpdateScript(float dt)
	{
		auto nativeView = GetEntities<TagComponent, NativeScriptComponent>();
		for (auto& natives : nativeView)
		{
			auto& tagComp = nativeView.get<TagComponent>(natives);
			if (tagComp.Active)
			{
				auto& nativeComp = nativeView.get<NativeScriptComponent>(natives);
				nativeComp.OnUpdateFunction(nativeComp.Instance, dt);
				if (m_SceneChanged)
					return;
			}
		}
	}

	void Scene::OnPostUpdateScript()
	{
		auto nativeView = m_Registry.view<TagComponent, NativeScriptComponent>();
		nativeView.each([&](auto entity, TagComponent& tag, NativeScriptComponent& nsc)
						{
							if (tag.Active) 
							{
								nsc.OnDestroyFunction(nsc.Instance);
							}
						});
	}

	void Scene::LoadPrefab(const std::string& path)
	{
		Database::LoadFromJson(path);
		auto& json = Database::GetJsonObject(path);
		for (json::iterator itEntityUUID = json.begin(); itEntityUUID != json.end(); ++itEntityUUID)
		{
			const std::string& entityUUID = itEntityUUID.key();
			fz::Entity entity = this->CreateEntityWithUUID("New Entity...", entityUUID);
			entity.m_UUID = entityUUID;
			EntitySerializer serialize(entity);
			serialize.Deserialize(json);
		}
		Database::Unload(path);
	}

	void Scene::OnCreateRuntimeInstance()
	{
		if (!s_World->IsLocked())
		{
			while (!m_LoadPrefabInstanceList.empty())
			{
				fz::Entity gen = m_LoadPrefabInstanceList.front();
				m_LoadPrefabInstanceList.pop_front();
				this->LoginPhysicsWorld(gen);
				if (gen.HasComponent<NativeScriptComponent>())
				{
					auto& dstNativeComp = gen.GetComponent<NativeScriptComponent>();
					dstNativeComp.Instance = dstNativeComp.CreateInstanceFunc();
					dstNativeComp.Instance->m_Entity = gen;
					dstNativeComp.OnCreateFunction(dstNativeComp.Instance);
				}
				m_PrefabInstancePool.insert({ gen.m_UUID, gen.m_Handle });
			}
		}
	}

	void Scene::OnUpdatePhysicsSystem(float dt)
	{
		if (m_SceneChanged)
			return;
		// 물리 시스템 업데이트
		if (s_World)
		{
			float timeStep = 1.0f / 60.0f; // 초당 60프레임
			const int32_t velocityIterations = 8;
			const int32_t positionIterations = 3;
			s_World->Step(timeStep, velocityIterations, positionIterations);

			auto view = m_Registry.view<RigidbodyComponent>();
			for (auto e : view)
			{
				Entity entity = { e, shared_from_this() };
				auto& tagComp = entity.GetComponent<TagComponent>();
				if (tagComp.Active == false)
					continue; // ** 비활성화시 로직 생략

				auto& transformComp = entity.GetComponent<TransformComponent>();
				auto& transform = transformComp.Transform;
				auto& rigid = entity.GetComponent<RigidbodyComponent>();
				b2Body* body = (b2Body*)rigid.RuntimeBody;
				if (body)
				{
					const sf::Vector2f& pixelBoxPos = Utils::MeterToPixel(body->GetPosition());
					const auto& bodyRot = Utils::RadianToDegree(body->GetAngle());
					if (entity.HasComponent<ParentEntityComponent>())
					{
						fz::Entity parent = entity.GetComponent<ParentEntityComponent>().ParentEntity;
						const sf::Transform& inverse = parent.GetWorldTransform().getInverse();
						transform.SetTranslate(inverse * pixelBoxPos);
						transform.SetRotation(bodyRot);
					}
					else
					{
						transform.SetTranslate(pixelBoxPos.x, pixelBoxPos.y);
						transform.SetRotation(bodyRot);
					}
				}
			}
		}
	}

	void Scene::OnUpdateCamera(OrthoCamera** dstCamera, sf::Transform& dstTransform)
	{
		if (m_SceneChanged)
			return;

		auto view = m_Registry.view<TagComponent, TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			const auto& [tag, transform, camera] = view.get<TagComponent, TransformComponent, CameraComponent>(entity);
			if (tag.Active == false)
				continue; // ** 비활성화시 로직 생략

			if (camera.Primary)
			{
				if (transform.IsChildRenderMode)
					dstTransform = transform.RenderTransform;
				else
					dstTransform = transform.Transform.GetRawTransform();
				*dstCamera = &camera.Camera;
				break;
			}
		}
	}

	void Scene::OnRenderEditorSprite(OrthoCamera* mainCamera)
	{
		// 스프라이트 렌더링
		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, m_FrameBuffer);
			// 스프라이트 렌더링
			{
				auto entities = GetEntities<TagComponent, TransformComponent, SpriteComponent>();
				for (auto handle : entities)
				{
					Entity entity = { handle, shared_from_this() };
					const auto& [tag, transform, Sprite] = entities.get<TagComponent, TransformComponent, SpriteComponent>(handle);
					if (tag.Active == false)
						continue; // ** 비활성화시 로직 생략

					Renderer2D::Draw(Sprite.SortingOrder, Sprite, entity.GetWorldTransform(), transform.AnimTransform);
				}
			}
			// Text 렌더링
			{
				auto entities = GetEntities<TagComponent, TransformComponent, TextComponent>();
				for (auto handle : entities)
				{
					Entity entity = { handle, shared_from_this() };
					const auto& [tag, transform, text] = entities.get<TagComponent, TransformComponent, TextComponent>(handle);
					if (tag.Active == false)
						continue; // ** 비활성화시 로직 생략
					Renderer2D::Draw(text.SortingOrder, text.Text, entity.GetWorldTransform(), transform.AnimTransform);
				}
			}
			// Debug Display Mode
			OnDrawDebugShape();
			Renderer2D::EndScene();
		}
		else
		{
			m_FrameBuffer->Clear();
		}
	}

	void Scene::OnRuntimeRenderDrawable(OrthoCamera* mainCamera, sf::Transform& transform)
	{
		if (m_SceneChanged)
			return;

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, transform, m_FrameBuffer); 

			// 스프라이트 렌더링
			{
				auto entities = GetEntities<TagComponent, TransformComponent, SpriteComponent>();
				for (auto handle : entities)
				{
					Entity entity = { handle, shared_from_this() };
					const auto& [tag, transform, Sprite] = entities.get<TagComponent, TransformComponent, SpriteComponent>(handle);
					if (tag.Active == false)
						continue; // ** 비활성화시 로직 생략

					Renderer2D::Draw(Sprite.SortingOrder, Sprite, entity.GetWorldTransform(), transform.AnimTransform);
				}
			}
			// Text 렌더링
			{
				auto entities = GetEntities<TagComponent, TransformComponent, TextComponent>();
				for (auto handle : entities)
				{
					Entity entity = { handle, shared_from_this() };
					const auto& [tag, transform, text] = entities.get<TagComponent, TransformComponent, TextComponent>(handle);
					if (tag.Active == false)
						continue; // ** 비활성화시 로직 생략
					Renderer2D::Draw(text.SortingOrder, text.Text, entity.GetWorldTransform(), transform.AnimTransform);
				}
			}
			// Debug Display Mode
			OnDrawDebugShape();
			Renderer2D::EndScene();
		}
		else
		{
			m_FrameBuffer->Clear();
		}
	}

	void Scene::OnDrawDebugShape()
	{
		if (!m_IsDebugMode)
			return;

		auto boxView = GetEntities<TagComponent, TransformComponent, BoxCollider2DComponent>();
		for (auto& handle : boxView)
		{
			const auto& [tag, transformComp, boxComp] = boxView.get<TagComponent, TransformComponent, BoxCollider2DComponent>(handle);
			if (tag.Active == false)
				continue; // ** 비활성화시 로직 생략

			sf::RectangleShape* rect = new sf::RectangleShape;
			rect->setOutlineColor(sf::Color::Green);
			rect->setOutlineThickness(1.0f);
			rect->setFillColor(sf::Color::Transparent);
			rect->setSize({ boxComp.Size.x * 2.0f, boxComp.Size.y * 2.0f });
			rect->setPosition({ boxComp.Size.x * -1.0f, boxComp.Size.y * -1.0f });
			fz::Entity entity = { handle, this->shared_from_this() };
			Renderer2D::PostDraw(rect, entity.GetWorldTransform());
		}
		auto edgeView = GetEntities<TagComponent, TransformComponent, EdgeCollider2DComponent>();
		for (auto& handle : edgeView)
		{
			const auto& [tag, transformComp, edgeComp] = edgeView.get<TagComponent, TransformComponent, EdgeCollider2DComponent>(handle);
			if (tag.Active == false)
				continue; // ** 비활성화시 로직 생략

			sf::Vector2f size = (edgeComp.EndPos - edgeComp.StartPos);
			sf::Vector2f pos = edgeComp.StartPos;
			sf::RectangleShape* line = new sf::RectangleShape;
			line->setOutlineColor(sf::Color::Green);
			line->setOutlineThickness(1.0f);
			line->setFillColor(sf::Color::Green);
			line->setSize(size);
			line->setPosition(pos);
			fz::Entity entity = { handle, this->shared_from_this() };
			Renderer2D::PostDraw(line, entity.GetWorldTransform());
		}
		// auto circleView = GetEntities<TransformComponent, CircleCollider2DComponent>();
	}

} // namespace fz