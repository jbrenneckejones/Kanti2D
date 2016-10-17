#ifndef KANTI2D_AUDIO

class Audio
{
public:

	inline Audio()
	{
		if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
		{
			printf("Audio wasn't able to initialize\n");
		}
	}

	inline Mix_Music* LoadMusic(string FileName)
	{
		if (Music.count(FileName) == 0)
		{
			Music[FileName] = Mix_LoadMUS(FileName.c_str());
		}

		return Music[FileName];
	}

	inline Mix_Chunk* LoadSound(string FileName)
	{
		if (Sounds.count(FileName) == 0)
		{
			Sounds[FileName] = Mix_LoadWAV(FileName.c_str());
		}

		return Sounds[FileName];
	}

	inline void PlayMusic(string FileName, bool32 OverridePlay = 0, bool32 ResumeIfPaused = 1)
	{
		Mix_Music* FoundMusic = LoadMusic(FileName);

		PlayMusic(FoundMusic, OverridePlay, ResumeIfPaused);
	}

	inline void PlayMusic(Mix_Music* MusicToPlay, bool32 OverridePlay = 0, bool32 ResumeIfPaused = 1, uint32 LoopCount = -1)
	{
		if (Mix_PlayingMusic() == 0 || OverridePlay == 1)
		{
			if (Mix_PlayMusic(MusicToPlay, LoopCount) == -1)
			{
				return;
			}
		}
		else if (Mix_PausedMusic() == 1 && ResumeIfPaused)
		{
			Mix_ResumeMusic();
		}
	}

	inline void PauseMusic()
	{
		Mix_PauseMusic();
	}

	inline void PlaySound(string FileName, uint32 Channel = -1, uint32 LoopCount = 0)
	{
		Mix_Chunk* Sound = LoadSound(FileName);

		PlaySound(Sound, Channel, LoopCount);
	}

	inline void PlaySound(Mix_Chunk* SoundEffect, uint32 Channel = -1, uint32 LoopCount = 0)
	{
		if (Mix_PlayChannel(Channel, SoundEffect, LoopCount) == -1)
		{
			return;
		}
	}

	std::map<string, Mix_Music*> Music;
	std::map<string, Mix_Chunk*> Sounds;
};

#define KANTI2D_AUDIO

#endif

