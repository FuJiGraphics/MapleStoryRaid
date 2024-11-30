#pragma once

#include <VegaEngine2.h>

namespace fz {

	class RawTimer 
	{
	public:
		RawTimer() = default;

		void Start(float time)
		{
			if (!this->Done())
				return;
			m_Time = time;
			m_ElapsedTime = 0.f;
		}

		void Update(float dt)
		{
			m_ElapsedTime += dt;
		}

		bool Done() const
		{
			return Utils::IsEqual(m_Time, m_ElapsedTime) || m_ElapsedTime > m_Time;
		}

	protected:
		float m_Time = 0;
		float m_ElapsedTime = 0;
	};

	class Timer
	{
	protected:
		using TimerPool = std::unordered_map<std::string, RawTimer>;
		
	public:
		Timer() = default;

		void Update(float dt)
		{
			for (auto& timer : m_Timers)
			{
				timer.second.Update(dt);
			}
		}

		RawTimer& operator[](const std::string& name)
		{
			return m_Timers[name];
		}

	private:
		TimerPool m_Timers;
	};

} // namespace fz