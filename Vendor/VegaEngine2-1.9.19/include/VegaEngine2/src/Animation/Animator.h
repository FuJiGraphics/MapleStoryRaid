#pragma once
#include <queue>

namespace fz {

	struct AnimationEvent
	{
		std::string id;
		int frame = 0;

		std::vector<std::function<void()>> actions;

		bool operator==(const AnimationEvent& other) const
		{
			return (id == other.id) && (frame == other.frame);
		}
	};

	struct AnimationEventHash
	{
		std::size_t operator()(const std::pair<std::string, int>& pair) const
		{
			std::size_t h1 = std::hash<std::string>()(pair.first);
			std::size_t h2 = std::hash<int>()(pair.second);
			return h1 ^ (h2 << 1);
		}
	};

	class Animator
	{
	protected:
		std::unordered_map<std::pair<std::string, int>, AnimationEvent, AnimationEventHash> events;
		std::queue<std::string> playeQueue;

		fz::Entity Entity;
		std::string currentTag;
		AnimationClip* currentClip;

		bool isPlaying = false;
		int currentFrame = 0;
		int totalFrame = 0;
		int checkFrame = 0;

		float frameDuration = 0.f;
		float accumTime = 0.f;
		float speed = 1.f;

	public:
		Animator() = default;
		~Animator() = default;

		void SetTarget(fz::Entity target)
		{
			Entity = target;
		}
		void AddEvent(const std::string& id, int frame, std::function<void()> action);
		void ClearEvent() { events.clear(); }

		void Update(float dt);

		bool IsPlaying(int frame) const { return currentFrame == frame; }
		bool IsPlaying() const { return false; }
		void SetSpeed(float speed)
		{
			this->speed = speed;
			checkFrame = this->speed > 0.f ? totalFrame : -1;
		}

		void Play(const std::string& clipId, bool clearQueue = true);
		void Play(AnimationClip* clip, bool clearQueue = true);

		void PlayQueue(const std::string& clipId);

		void Stop();

		//void Pause(bool pause);

		void SetFrame(const AnimationFrame& frame);

		const std::string& GetCurrentClipId() const
		{
			if (currentClip == nullptr)
				return "";
			return currentClip->id;
		}

		AnimationClip* GetCurrentClip() const 
		{
			return currentClip;
		}
	};

} // namespace fz