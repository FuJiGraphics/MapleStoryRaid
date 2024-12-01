#include "pch.h"
#include "SceneManager.h"

namespace fz {

	void SceneManager::SetContext(Shared<Scene>& scene)
	{
		m_ActiveScene = scene;
	}

} // namespace fz
