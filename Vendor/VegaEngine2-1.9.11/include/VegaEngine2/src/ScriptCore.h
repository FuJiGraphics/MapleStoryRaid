#pragma once

class BindScriptBase
{
public:
	virtual void Bind(const std::string& path, fz::Shared<fz::Scene>& currScene) = 0;

	static std::vector<BindScriptBase*>& GetInstance()
	{
		static std::vector<BindScriptBase*> s_BindScriptBase;
		return s_BindScriptBase;
	}
};

#define BIND_SCRIPT(tag, entity, script)												\
namespace fz {																			\
	class bindClass##tag : public BindScriptBase										\
	{																					\
	public:																				\
		void Bind(const std::string& path, fz::Shared<fz::Scene>& currScene) override	\
		{																				\
			auto Entity = currScene->GetEntityFromTag(entity);							\
			if (Entity && !Entity.HasComponent<NativeScriptComponent>())				\
				Entity.AddComponent<NativeScriptComponent>().Bind<##script>();			\
		}																				\
	};																					\
class bindClassBind##tag																\
{																						\
public:																					\
	bindClassBind##tag()																\
	{																					\
		auto& base = BindScriptBase::GetInstance();										\
		base.push_back(new bindClass##tag);												\
	}																					\
};																						\
inline bindClassBind##tag bindClassBindObj##tag;										\
}

