#pragma once

#include "Core/Core.h"
#include "LayerPool.h"

namespace sf {
	class Event;
}

namespace fz {

	// Forwards
	class Layer;
	class LayerPool;

	class LayerStack
	{
	protected:
		// Using
		using Super	= fz::LayerStack;
		using Iter = std::vector<Layer*>::iterator;
		using Const_iter = std::vector<Layer*>::const_iterator;
		using Reverse_iter = std::vector<Layer*>::reverse_iterator;
		using Const_reverse_iter = std::vector<Layer*>::const_reverse_iterator;
		// Delete
		FZ_DELETE_COPY(LayerStack)

	public:
		LayerStack(Shared<LayerPool>& linkPool);
		virtual ~LayerStack();

		void Release();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);

		inline unsigned int size() const			{ return m_Layers.size(); }
		inline bool empty() const					{ return m_Layers.empty(); }

		inline Iter begin()							{ return m_Layers.begin(); }
		inline Iter end()							{ return m_Layers.end(); }
		inline Reverse_iter rbegin()				{ return m_Layers.rbegin(); }
		inline Reverse_iter rend()					{ return m_Layers.rend(); }

		inline Const_iter begin() const				{ return m_Layers.begin(); }
		inline Const_iter end() const				{ return m_Layers.end(); }
		inline Const_reverse_iter rbegin() const	{ return m_Layers.rbegin(); }
		inline Const_reverse_iter rend() const		{ return m_Layers.rend(); }

	private:
		std::vector<Layer*>		m_Layers;
		Shared<LayerPool>&		m_Pool;
		unsigned int			m_InsertIndex;
	};
} // namespace fz