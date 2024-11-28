#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "VegaScript.h"
#include "EditorCamera.h"
#include "EntitySerializer.h"
#include "CollisionHandler.h"

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
			dstTagComp.Tag = srcTagComp.Tag;
		}
		// Transform 
		{
			auto& srcTransformComp = src.GetComponent<TransformComponent>();
			auto& dstTransformComp = dst.GetComponent<TransformComponent>();
			if (!isRootTransform) // 지정된 Transform일 경우 복사 제외 (차일드 엔티티일 경우)
				dstTransformComp.Transform = srcTransformComp.Transform;
			dstTransformComp.AnimTransform = srcTransformComp.AnimTransform;
			dstTransformComp.RenderTransform = srcTransformComp.RenderTransform;
			dstTransformComp.IsChildRenderMode = srcTransformComp.IsChildRenderMode;
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
			// TODO: 스크립트 복사
			if (src.HasComponent<NativeScriptComponent>())
			{
				auto& srcNativeComp = src.GetComponent<NativeScriptComponent>();
				auto& dstNativeComp = dst.AddComponent<NativeScriptComponent>();
				dstNativeComp.Instance = srcNativeComp.CreateInstanceFunc();
				dstNativeComp.Instance->m_Entity = dst;
				dstNativeComp.CreateInstanceFunc = srcNativeComp.CreateInstanceFunc;
				dstNativeComp.DeleteInstanceFunc = srcNativeComp.DeleteInstanceFunc;
				dstNativeComp.OnCreateFunction = srcNativeComp.OnCreateFunction;
				dstNativeComp.OnDestroyFunction = srcNativeComp.OnDestroyFunction;
				dstNativeComp.OnUpdateFunction = srcNativeComp.OnUpdateFunction;
			}

			LoginPhysicsWorld(dst);

			m_PrefabInstancePool.insert({ dst.m_UUID, dst.m_Handle });
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
		auto it = m_EntityPool.find(entity.m_UUID);
		if (it != m_EntityPool.end())
		{
			m_EntityPool.erase(it);
		}
		m_Registry.destroy(entity.m_Handle);
		entity.m_Handle = entt::null;
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
		auto& transformComp = entity.GetComponent<TransformComponent>();
		auto& rigidBodyComp = entity.GetComponent<RigidbodyComponent>();
		auto& transform = transformComp.Transform;

		const b2Vec2& meterPos = Utils::PixelToMeter(entity.GetWorldPosition());

		b2BodyDef bodyDef;
		bodyDef.type = ToBox2dBodyType(rigidBodyComp.RigidType);
		bodyDef.position.Set(meterPos.x, meterPos.y);
		bodyDef.angle = Utils::DegreeToRadian(transform.GetRotation());
		b2Body* body = s_World->CreateBody(&bodyDef);
		body->SetFixedRotation(rigidBodyComp.FixedRotation);
		rigidBodyComp.RuntimeBody = body;

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& collider = entity.GetComponent<BoxCollider2DComponent>();

			const b2Vec2& meterBoxSize = Utils::PixelToMeter({ std::abs(collider.Size.x * transform.GetScale().x),
															   std::abs(collider.Size.y * transform.GetScale().y) });
			if (meterBoxSize.x <= 0.0f || meterBoxSize.y <= 0.0f)
			{
				FZLOG_WARN("Collider 생성 실패, Box Size 크기를 잘못 설정하였습니다. {0}, {1},", meterBoxSize.x, meterBoxSize.y);
				return;
			}

			auto& tag = entity.GetComponent<TagComponent>();
			FZLOG_DEBUG("UUID = {0}, Tag = {1}", entity.m_UUID, tag.Tag);

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
			collider.RuntimeFixture = body->CreateFixture(&fixtureDef);
		}
	}

	void Scene::OnUpdateEditor(float dt, EditorCamera& editorCamera)
	{
		this->OnUpdateChildEntity();
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
		this->OnUpdateScript(dt);
		this->OnUpdateChildEntity();
		this->OnUpdateCamera(&camera, transform);
		this->OnUpdatePhysicsSystem(dt);
		this->OnRenderRuntimeSprite(camera, transform);
	}

	void Scene::OnPostUpdate()
	{
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

	void Scene::ReleasePrefabInstancies()
	{
		for (auto& instance : m_PrefabInstancePool)
		{
			fz::Entity deleteEntity = { instance.first, instance.second, shared_from_this() };
			this->DeleteEntity(deleteEntity);
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
		auto view = m_Registry.view<TagComponent>();
		for (auto& it : m_EntityPool)
		{
			Entity entity = { it.first, it.second, shared_from_this() };
			std::string targetTag = entity.GetComponent<TagComponent>().Tag;
			if (targetTag == tag)
				return entity;
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
		newEntity.AddComponent<PrefabInstance>();

		auto& transformComp = newEntity.GetComponent<TransformComponent>();
		transformComp.Transform = transform;
		CopyEntityForPrefab(newEntity, entity, true);

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
								if (tag.Active)
								{
									nsc.OnCreateFunction(nsc.Instance);
								}
							}
						});
	}

	void Scene::OnUpdateScript(float dt)
	{
		auto nativeView = m_Registry.view<TagComponent, NativeScriptComponent>();
		if (nativeView.size_hint())
		nativeView.each([&](auto entity, TagComponent& tag, NativeScriptComponent& nsc)
						{
							if (tag.Active)
							{
								nsc.OnUpdateFunction(nsc.Instance, dt);
							}
						});
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

	void Scene::OnUpdateChildEntity()
	{
		// 차일드 엔티티 트랜스폼 업데이트
		auto rootEntityView = m_Registry.view<RootEntityComponent>();
		for (auto& rootEntity : rootEntityView)
		{
			auto& rootComp = m_Registry.get<RootEntityComponent>(rootEntity);
			if (rootComp.RootEntity.HasComponent<ChildEntityComponent>())
			{
				sf::Transform rootTransform = rootComp.RootEntity.GetComponent<TransformComponent>().Transform;
				auto& childComp = rootComp.RootEntity.GetComponent<ChildEntityComponent>();
				for (auto& child : childComp.CurrentChildEntities)
				{
					auto& tagComp = child.GetComponent<TagComponent>();
					if (tagComp.Active == false)
						continue; // ** 비활성화시 로직 생략
					UpdateTransformChilds(rootTransform, child);
				}
			}
		}
	}

	void Scene::UpdateTransformChilds(const sf::Transform& parentTransform, fz::Entity child)
	{
		// 차일드 엔티티 트랜스폼 업데이트
		TransformComponent& childTransform = child.GetComponent<TransformComponent>();
		childTransform.IsChildRenderMode = true;
		childTransform.RenderTransform = parentTransform * childTransform.Transform.GetRawTransform();

		if (child.HasComponent<ChildEntityComponent>())
		{
			auto childComp = child.GetComponent<ChildEntityComponent>();
			for (auto& childs : childComp.CurrentChildEntities)
			{
				auto& tagComp = childs.GetComponent<TagComponent>();
				if (tagComp.Active == false)
					continue; // ** 비활성화시 로직 생략
				UpdateTransformChilds(childTransform.RenderTransform, childs);
			}
		}
	}

	void Scene::OnUpdatePhysicsSystem(float dt)
	{
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
					transform.SetTranslate(pixelBoxPos.x, pixelBoxPos.y);
					transform.SetRotation(bodyRot);
				}
			}
		}
	}

	void Scene::OnUpdateCamera(OrthoCamera** dstCamera, sf::Transform& dstTransform)
	{
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

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent, TagComponent>);
			for (auto entity : group)
			{
				const auto& [transform, sprite, tag] = group.get<TransformComponent, SpriteComponent, TagComponent>(entity);
				if (tag.Active == false)
					continue; // ** 비활성화시 로직 생략

				if (transform.IsChildRenderMode)
					Renderer2D::Draw(sprite.SortingOrder, sprite, transform.RenderTransform);
				else
					Renderer2D::Draw(sprite.SortingOrder, sprite, transform.Transform * transform.AnimTransform);
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

	void Scene::OnRenderRuntimeSprite(OrthoCamera* mainCamera, sf::Transform& transform)
	{
		// 스프라이트 렌더링
		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, transform, m_FrameBuffer);
			auto entities = GetEntities< TransformComponent, SpriteComponent, TagComponent>();
			for (auto entity : entities)
			{
				const auto& [transform, sprite, tag] = entities.get<TransformComponent, SpriteComponent, TagComponent>(entity);
				if (tag.Active == false)
					continue; // ** 비활성화시 로직 생략

				if (transform.IsChildRenderMode)
					Renderer2D::Draw(sprite.SortingOrder, sprite, transform.RenderTransform, transform.AnimTransform);
				else
					Renderer2D::Draw(sprite.SortingOrder, sprite, transform.Transform, transform.AnimTransform);
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
			Renderer2D::Draw(rect, entity.GetWorldTransform());
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
			Renderer2D::Draw(line, entity.GetWorldTransform());
		}
		// auto circleView = GetEntities<TransformComponent, CircleCollider2DComponent>();
	}

} // namespace fz