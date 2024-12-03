#include "pch.h"
#include "AnimationClip.h"

namespace fz {

	namespace {
		static AnimationLoopTypes ToLoopType(const std::string& str)
		{
			if (str == "Single")
				return AnimationLoopTypes::Single;
			if (str == "Loop")
				return AnimationLoopTypes::Loop;
			if (str == "PingPong")
				return AnimationLoopTypes::PingPong;
			FZLOG_ASSERT(false, "Failed to converted a type");
			return AnimationLoopTypes::Single;
		}
	}

	bool AnimationClip::loadFromFile(const std::string& filePath)
	{
		if (!Utils::CanFileOpen(filePath))
			return false;

		Database::LoadFromJson(filePath);
		auto& json = Database::GetJsonObject(filePath);
		auto& target = json["AnimationClip"];

		std::string loopStr;
		if (!target["Speed"].is_null())
			Speed = target["Speed"];
		id = target["ClipName"];
		loopStr = target["LoopType"];
		path = target["TexturePath"];
		int size = target["FrameCount"];
		auto& translate = target["Transform"]["Translate"];
		auto& Rotation = target["Transform"]["Rotation"];
		auto& Scale = target["Transform"]["Scale"];
		transform.SetTranslate({ translate[0], translate[1] });
		transform.SetRotation(Rotation);
		transform.SetScale({ Scale[0], Scale[1] });

		loopType = ToLoopType(loopStr);
		fps = size;
		TEXTURE_MGR.Load(path);
		
		for (int i = 0; i < size; ++i)
		{
			std::string index = std::to_string(i);
			auto& targeti = target[index.c_str()];
			const auto& border = targeti["Border"];
			const auto& color = targeti["Color"];
			const auto& rect = targeti["Rect"];

			sf::Sprite newSprite;
			newSprite.setTexture(TEXTURE_MGR.Get(path));
			newSprite.setTextureRect({ rect[0], rect[1], rect[2], rect[3] });
			newSprite.setPosition({ border[0], border[1] });
			newSprite.setColor({ color[0], color[1], color[2], color[3] });
			Utils::SetOrigin(newSprite, Origins::MC);
			frames.push_back(newSprite);
		}
		Database::Unload(filePath);
		return true;
	}

} // namespace fz