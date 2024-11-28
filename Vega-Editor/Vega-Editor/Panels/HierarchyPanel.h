#pragma once
#include <VegaEngine2.h>

namespace fz {

	class HierarchyPanel
	{
	public:
		HierarchyPanel(const Shared<Scene>& scene);

		void SetContext(const Shared<Scene>& scene);

		void OnImGuiRender();

	protected:
		bool DrawTreeNode(fz::Entity& entity, const char* tag);
		void DrawSceneComponents(fz::Entity& entity);
		void DeleteChildEntities(fz::Entity& entity);

		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

	private:
		Shared<Scene> m_Context;
		fz::Entity m_SelectionContext;
		bool m_OnEntityRemove;
	};
}

#include "HierarchyPanel.hpp"