#include "pch.h"
#include "LayerStack.h"

namespace fz {

	LayerStack::LayerStack(Shared<LayerPool>& linkPool)
		: m_Layers()
		, m_InsertIndex(0)
		, m_Pool(linkPool)
	{
		// Empty
	}

	LayerStack::~LayerStack()
	{
		this->Release();
	}

	void LayerStack::Release()
	{
		if (m_Layers.empty())
			return;

		for (Layer* layer : m_Layers)
		{
			if (layer != nullptr)
			{
				layer->OnDetach();
				m_Pool->Return(layer);
			}
		}
		m_Layers.clear();
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_InsertIndex, layer);
		m_InsertIndex++;
		layer->OnAttach();
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_InsertIndex, layer);
		if (it != m_Layers.begin() + m_InsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(it);
			m_InsertIndex--;
			m_Pool->Return(layer);
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layers.begin() + m_InsertIndex, m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			overlay->OnDetach();
			m_Layers.erase(it);
			m_Pool->Return(overlay);
		}
	}

} // namespace fz