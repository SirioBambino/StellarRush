#pragma once
#include <windows.h>									// Header File For The Windows Library
#include "./include/fmod_studio/fmod.hpp"
#include "./include/fmod_studio/fmod_errors.h"
#include <vector>

class CAudio
{
public:
	CAudio();
	~CAudio();

	bool Initialise();

	bool LoadEventSound(const char *filename);
	bool PlayEventSound(int index);

	bool LoadMusicStream(const char *filename);
	bool PlayMusicStream();
	bool StopMusicStream();

	void SetMusicVolume(float volume) { m_musicVolume = volume; }
	void SetEventVolume(float volume) { m_eventVolume = volume; }

	void Update();

private:
		
	void FmodErrorCheck(FMOD_RESULT result);

	FMOD_RESULT result;
	FMOD::System *m_FmodSystem;	// the global variable for talking to FMOD
	
	FMOD::Sound* m_eventSound;
	FMOD::Channel* m_eventChannel;
	std::vector<FMOD::Sound*> m_eventSounds;
	
	FMOD::Sound *m_music;
	FMOD::Channel* m_musicChannel;

	float m_eventVolume;
	float m_musicVolume;
};
