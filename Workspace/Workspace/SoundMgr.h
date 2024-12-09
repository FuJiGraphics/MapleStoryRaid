#pragma once

#include <VegaEngine2.h>
#include <list>

namespace fz {

	class SoundMgr : public Singleton<SoundMgr>
	{
		friend Singleton<SoundMgr>;

	protected:
		SoundMgr() = default;
		virtual ~SoundMgr() = default;

		SoundMgr(const SoundMgr& other) = delete;
		SoundMgr& operator=(const SoundMgr& other) = delete;

		sf::Sound bgm;

		std::list<sf::Sound*> playing;
		std::list<sf::Sound*> waiting;

		float sfxVolume = 100.f;
		float bgmVolume = 100.f;

	public:

		void SetBgmVolume(float v)
		{
			bgmVolume = v;
			bgm.setVolume(bgmVolume);
		}

		void Init(int totalChannels)
		{
			for (int i = 0; i < totalChannels; ++i)
			{
				waiting.push_back(new sf::Sound());
			}
		}

		void Release()
		{
			for (auto sound : waiting)
			{
				delete sound;
			}
			waiting.clear();
			for (auto sound : playing)
			{
				delete sound;
			}
			waiting.clear();
		}

		void Update(float dt)
		{
			auto it = playing.begin();
			while (it != playing.end())
			{
				if ((*it)->getStatus() == sf::Sound::Stopped)
				{
					waiting.push_back(*it);
					playing.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		void PlayBgm(std::string id, bool loop)
		{
			PlayBgm(SOUNDBUFFER_MGR.Get(id), loop);
		}

		void PlayBgm(sf::SoundBuffer& buffer, bool loop)
		{
			bgm.stop();
			//bgm.setVolume(bgmVolume);
			bgm.setLoop(loop);
			bgm.setBuffer(buffer);
			bgm.play();
		}

		void StopBgm()
		{
			bgm.stop();
		}

		void PlaySfx(std::string id, bool loop)
		{
			PlaySfx(SOUNDBUFFER_MGR.Get(id), loop);
		}

		void PlaySfx(sf::SoundBuffer& buffer, bool loop)
		{
			sf::Sound* sound = nullptr;

			if (waiting.empty())
			{
				sound = playing.front();
				playing.pop_front();
				sound->stop();
			}
			else
			{
				sound = waiting.front();
				waiting.pop_front();
			}

			sound->setVolume(sfxVolume);
			sound->setBuffer(buffer);
			sound->setLoop(loop);
			sound->play();
			playing.push_back(sound);
		}

		void SetSfxVolume(float v)
		{
			sfxVolume = v;
			for (auto sound : playing)
			{
				sound->setVolume(sfxVolume);
			}
		}

		void StopAllSfx()
		{
			for (auto sound : playing)
			{
				sound->stop();
				waiting.push_back(sound);
			}
			playing.clear();
		}
	};

#define SOUND_MGR (fz::SoundMgr::Instance())
}
