#include "Audio.h"

#pragma comment(lib, "lib/fmod_vc.lib")

CAudio::CAudio()
{}

CAudio::~CAudio()
{}

bool CAudio::Initialise()
{
	// Create an FMOD system
	result = FMOD::System_Create(&m_FmodSystem);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	// Initialise the system
	result = m_FmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	m_eventVolume = 1.0f;
	m_musicVolume = 1.0f;

	return true;
}

// Load an event sound
bool CAudio::LoadEventSound(const char *filename)
{
	result = m_FmodSystem->createSound(filename, NULL, 0, &m_eventSound);
	m_eventSounds.push_back(m_eventSound);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	return true;
}

// Play an event sound
bool CAudio::PlayEventSound(int index)
{
	result = m_FmodSystem->playSound(m_eventSounds[index], NULL, false, &m_eventChannel);
	FmodErrorCheck(result);

	//m_eventChannel->setVolume(m_eventVolume);

	if (result != FMOD_OK)
		return false;

	return true;
}


// Load a music stream
bool CAudio::LoadMusicStream(const char *filename)
{
	result = m_FmodSystem->createStream(filename, NULL | FMOD_LOOP_NORMAL, 0, &m_music);
	FmodErrorCheck(result);

	if (result != FMOD_OK) 
		return false;

	return true;
}

// Play a music stream
bool CAudio::PlayMusicStream()
{
	result = m_FmodSystem->playSound(m_music, NULL, false, &m_musicChannel);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;

	return true;
}

// Stop a music stream
bool CAudio::StopMusicStream()
{
	result = m_musicChannel->stop();
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;

	return true;
}

// Check for error
void CAudio::FmodErrorCheck(FMOD_RESULT result)
{						
	if (result != FMOD_OK) {
		const char *errorString = FMOD_ErrorString(result);
		// MessageBox(NULL, errorString, "FMOD Error", MB_OK);
		// Warning: error message commented out -- if headphones not plugged into computer in lab, error occurs
	}
}

void CAudio::Update()
{
	m_FmodSystem->update();

	m_eventChannel->setVolume(m_eventVolume);
	m_musicChannel->setVolume(m_musicVolume);
}
