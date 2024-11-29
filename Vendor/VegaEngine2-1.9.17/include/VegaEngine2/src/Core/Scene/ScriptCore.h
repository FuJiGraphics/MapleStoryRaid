#pragma once

namespace fz {			

	template <typename T>
	class BindScript									
	{																				
	public:			
		static void Bind(const std::string& tag, Shared<Scene>& scene)
		{
			if (scene)
			{
				auto Entity = scene->GetEntityFromTag(tag);
				if (Entity)
				{
					if (Entity.HasComponent<NativeScriptComponent>())
					{
						Entity.RemoveComponent<NativeScriptComponent>();
					}
					Entity.AddComponent<NativeScriptComponent>().Bind<T>();
				}
			}
		}
	};					

} // namespace fz

#define BIND_SCRIPT(tag, className)	BindScript<className>::Bind(tag, scene);	     
