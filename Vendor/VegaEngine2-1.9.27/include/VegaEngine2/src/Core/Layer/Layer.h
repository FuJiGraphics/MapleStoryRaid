#pragma once

namespace fz {

	class Event;

	class Layer
	{
	protected:
		// Using
		using Super = fz::Layer;
		// Delete
		FZ_DELETE_COPY(Layer)

	public:
		Layer(const std::string& name);
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float dt) {}
		virtual void OnEvent(fz::Event& ev) {}
		virtual void OnUI() {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string	m_DebugName;
	};

} // namespace fz