#pragma once

namespace fz {
	enum class AnimationLoopTypes
	{
		Single,
		Loop,
		PingPong,
	};

	struct AnimationFrame
	{
		sf::Sprite sprite;

		AnimationFrame() = default;
		AnimationFrame(const sf::Sprite& sprite)
			: sprite(sprite)
		{
			// Default;
		}
	};

	struct AnimationClip
	{
		std::string id;
		std::string path;
		float Speed = 1.0f;

		AnimationLoopTypes loopType = AnimationLoopTypes::Loop;
		fz::Transform transform;
		int fps = 30;

		std::vector<AnimationFrame> frames;

		bool loadFromFile(const std::string& filePath);
	};

} // namespace fz
