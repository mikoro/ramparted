#ifndef RAMPARTED_SOUNDMANAGER
#define RAMPARTED_SOUNDMANAGER

#include <map>
#include <string>

namespace FMOD
{
	class Sound;
	class System;
}

/// Manager encapsulating FMOD.
class SoundManager
{
public:

	/// Construct a new instance.
	SoundManager();

	/// Initialize the sound manager.
	/// This must be called before doing anything else.
	void initialize();

	/// Updates the underlying sound framework (FMOD atm).
	/// This shoul dbe called every logic update.
	void update();

	/// Release all resources.
	void shutdown();

	/// Load a sound from file.
	/// @param filename Path to the file to be loaded.
	/// @param identifier Unique identifier for the sound.
	/// @param stream Should the be streamed (good for music).
	void load(std::string filename, std::string identifier, bool stream = false);

	/// Play an already loaded sound.
	/// @param identifier Identifier for the sound (given with the load function).
	/// @param loop Should the sound loop forever.
	void play(std::string identifier, bool loop = false);
	
	/// Enable or disable sound output.
	/// This will only affect the play-function, load works like usually.
	/// @param state True to mute, false to unmute.
	void setMuted(bool state);

private:

	FMOD::System* fmodSystem;
	std::map<std::string, FMOD::Sound*> sounds;
	bool muted;
};

#endif
