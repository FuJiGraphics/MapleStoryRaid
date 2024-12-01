#pragma once

namespace fz {

	class SceneManager
	{
	public:
		void SetContext(Shared<Scene>& scene);


	private:
		Shared<Scene> m_ActiveScene;
	};


} // namespace fz
