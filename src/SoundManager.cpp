#include "SoundManager.h"
#include "GameEngine.h"

#include <fmodex/fmod.hpp>
#include <fmodex/fmod_errors.h>
#include <Ogre.h>

void checkFmodResult(FMOD_RESULT result)
{
	if(result != FMOD_OK)
	{
		std::string message(FMOD_ErrorString(result));
		GameEngine::getSingleton().getLog()->logMessage("FMOD error: " + message, Ogre::LML_CRITICAL);
		throw std::runtime_error("FMOD error:" + message);
	}
}

SoundManager::SoundManager()
{
	fmodSystem = 0;
	muted = false;
}

void SoundManager::initialize()
{
	Ogre::Log* log = GameEngine::getSingleton().getLog();

	log->logMessage("Creating FMOD system", Ogre::LML_NORMAL);
	checkFmodResult(FMOD::System_Create(&fmodSystem));
	
	log->logMessage("Checking FMOD version", Ogre::LML_NORMAL);
	
	unsigned int version;
	checkFmodResult(fmodSystem->getVersion(&version));
	
	if(version < FMOD_VERSION)
	{
		log->logMessage("You are using an old version of FMOD. This program requires FMOD >= 4.08.07.");
		throw std::runtime_error("You are using an old version of FMOD. This program requires FMOD >= 4.08.07.");
	}
	
	log->logMessage("Setting FMOD output type", Ogre::LML_NORMAL);
	
#ifdef WIN32
	checkFmodResult(fmodSystem->setOutput(FMOD_OUTPUTTYPE_DSOUND));
#else
	checkFmodResult(fmodSystem->setOutput(FMOD_OUTPUTTYPE_ALSA));
#endif

	log->logMessage("Initializing FMOD system", Ogre::LML_NORMAL);
	checkFmodResult(fmodSystem->init(32, FMOD_INIT_NORMAL, 0));
}

void SoundManager::update()
{
	checkFmodResult(fmodSystem->update());
}

void SoundManager::shutdown()
{
	GameEngine::getSingleton().getLog()->logMessage("Shutting down FMOD system", Ogre::LML_NORMAL);
	
	for(std::map<std::string, FMOD::Sound*>::iterator it = sounds.begin(); it != sounds.end(); ++it)
	{
		it->second->release();
		it->second = 0;
	}
	
	sounds.clear();
	
	if(fmodSystem)
	{
		fmodSystem->close();
		fmodSystem->release();
		fmodSystem = 0;
	}
}

void SoundManager::load(std::string filename, std::string identifier, bool stream)
{
	GameEngine::getSingleton().getLog()->logMessage("Loading sound from " + filename, Ogre::LML_NORMAL);
	
	FMOD::Sound* sound;
	checkFmodResult(fmodSystem->createSound(filename.c_str(), FMOD_SOFTWARE | (stream ? FMOD_CREATESTREAM : 0), 0, &sound));
	sounds[identifier] = sound;
}

void SoundManager::play(std::string identifier, bool loop)
{
	if(muted)
		return;
		
	FMOD::Channel* channel;
	checkFmodResult(fmodSystem->playSound(FMOD_CHANNEL_FREE, sounds[identifier], loop, &channel));
	
	if(loop)
	{
		checkFmodResult(channel->setMode(FMOD_LOOP_NORMAL));
		checkFmodResult(channel->setPaused(false));
	}
}

void SoundManager::setMuted(bool state)
{
	muted = state;
}
