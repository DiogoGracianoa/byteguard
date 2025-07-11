#include "AudioSystem.h"
#include <filesystem>
#include "SDL.h"
#include "SDL_mixer.h"

SoundHandle SoundHandle::Invalid;

// Create the AudioSystem with specified number of channels
// (Defaults to 8 channels)
AudioSystem::AudioSystem(const int numChannels)
{
	if (Mix_OpenAudio(44100,
	                  MIX_DEFAULT_FORMAT,
	                  2,
	                  2048) < 0)
	{
		SDL_Log("Error to start SDL_mixer: %s", Mix_GetError());
	}

	Mix_AllocateChannels(numChannels);

	mChannels.resize(numChannels);
}

// Destroy the AudioSystem
AudioSystem::~AudioSystem()
{
	for (const auto &[_, second]: mSounds) { Mix_FreeChunk(second); }

	mSounds.clear();

	Mix_CloseAudio();
}

// Updates the status of all the active sounds every frame
void AudioSystem::Update(float deltaTime)
{
	for (size_t i = 0; i < mChannels.size(); ++i)
	{
		if (SoundHandle &handle = mChannels[i]; handle.IsValid())
		{
			if (Mix_Playing(static_cast<int>(i)) == 0)
			{
				mHandleMap.erase(handle);
				handle.Reset();
			}
		}
	}
}

// Plays the sound with the specified name and loops if looping is true
// Returns the SoundHandle which is used to perform any other actions on the
// sound when active
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
SoundHandle AudioSystem::PlaySound(const std::string &soundName,
                                   const bool looping)
{
	// Get the sound with the given name
	Mix_Chunk *sound = GetSound(soundName);

	if (!sound)
	{
		SDL_Log("Error: Sound '%s' not found or failed to load.",
		        soundName.c_str());
		return SoundHandle::Invalid;
	}

	int availableChannel = -1;
	for (size_t i = 0; i < mChannels.size(); ++i)
	{
		if (!mChannels[i].IsValid())
		{
			availableChannel = static_cast<int>(i);
			break;
		}
	}

	if (availableChannel == -1)
	{
		for (auto it = mHandleMap.begin(); it != mHandleMap.end(); ++it)
		{
			if (it->second.mSoundName == soundName)
			{
				availableChannel = it->second.mChannel;
				SDL_Log("Stopping repeating sound '%s' on channel %d",
				        soundName.c_str(), availableChannel);
				mHandleMap.erase(it);
				break;
			}
		}
	}

	if (availableChannel == -1)
	{
		for (auto it = mHandleMap.begin(); it != mHandleMap.end(); ++it)
		{
			if (it->second.mIsLooping)
			{
				availableChannel = it->second.mChannel;
				SDL_Log("Stopping looping sound '%s' on channel %d",
				        it->second.mSoundName.c_str(), availableChannel);
				StopSound(it->first);
				break;
			}
		}
	}

	if (availableChannel == -1 && !mHandleMap.empty())
	{
		auto oldest = mHandleMap.begin();
		availableChannel = oldest->second.mChannel;
		SDL_Log("Stopping older sound '%s' on channel %d",
		        oldest->second.mSoundName.c_str(), availableChannel);
		StopSound(oldest->first);
	}

	if (availableChannel == -1)
	{
		SDL_Log("Error: No channels available to play '%s'", soundName.c_str());
		return SoundHandle::Invalid;
	}

	++mLastHandle;

	HandleInfo info;
	info.mSoundName = soundName;
	info.mChannel = availableChannel;
	info.mIsLooping = looping;
	info.mIsPaused = false;

	mHandleMap[mLastHandle] = info;
	mChannels[availableChannel] = mLastHandle;

	const int loopCount = looping ? -1 : 0;
	Mix_PlayChannel(availableChannel, sound, loopCount);

	return mLastHandle;
}

// Stops the sound if it is currently playing
void AudioSystem::StopSound(const SoundHandle sound)
{
	const auto it = mHandleMap.find(sound);
	if (it == mHandleMap.end())
	{
		SDL_Log("Error: Attempt to stop sound with invalid handle (%s)",
		        sound.GetDebugStr());
		return;
	}

	const int channel = it->second.mChannel;

	Mix_HaltChannel(channel);

	mHandleMap.erase(it);

	mChannels[channel].Reset();
}

// Pauses the sound if it is currently playing
void AudioSystem::PauseSound(const SoundHandle sound)
{
	const auto it = mHandleMap.find(sound);
	if (it == mHandleMap.end())
	{
		SDL_Log("Error: Attempt to resume sound with invalid handle (%s)",
		        sound.GetDebugStr());
		return;
	}

	if (!it->second.mIsPaused)
	{
		const int channel = it->second.mChannel;
		Mix_Pause(channel);
		it->second.mIsPaused = true;
	}
}

// Resumes the sound if it is currently paused
void AudioSystem::ResumeSound(const SoundHandle sound)
{
	const auto it = mHandleMap.find(sound);
	if (it == mHandleMap.end())
	{
		SDL_Log("Error: Attempt to resume sound with invalid handle (%s)",
		        sound.GetDebugStr());
		return;
	}

	if (it->second.mIsPaused)
	{
		const int channel = it->second.mChannel;
		Mix_Resume(channel);
		it->second.mIsPaused = false;
	}
}

// Returns the current state of the sound
SoundState AudioSystem::GetSoundState(const SoundHandle sound)
{
	if (mHandleMap.find(sound) == mHandleMap.end())
	{
		return SoundState::Stopped;
	}

	if (mHandleMap[sound].mIsPaused) { return SoundState::Paused; }

	return SoundState::Playing;
}

// Stops all sounds on all channels
void AudioSystem::StopAllSounds()
{
	Mix_HaltChannel(-1);

	for (auto &mChannel: mChannels) { mChannel.Reset(); }

	mHandleMap.clear();
}

// Cache all sounds under Assets/Sounds
void AudioSystem::CacheAllSounds()
{
#ifndef __clang_analyzer__
	std::error_code ec{};
	for (const auto &rootDirEntry: std::filesystem::directory_iterator
	     {"Assets/Sounds", ec})
	{
		if (std::string extension = rootDirEntry.path().extension().string();
			extension == ".ogg" || extension == ".wav")
		{
			std::string fileName = rootDirEntry.path().stem().string();
			fileName += extension;
			CacheSound(fileName);
		}
	}
#endif
}

// Used to preload the sound data of a sound
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
void AudioSystem::CacheSound(const std::string &soundName)
{
	GetSound(soundName);
}

// If the sound is already loaded, returns Mix_Chunk from the map.
// Otherwise, will attempt to load the file and save it in the map.
// Returns nullptr if sound is not found.
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
Mix_Chunk *AudioSystem::GetSound(const std::string &soundName)
{
	std::string fileName = "../Assets/Sounds/";
	fileName += soundName;

	Mix_Chunk *chunk = nullptr;
	if (const auto iter = mSounds.find(fileName);
		iter != mSounds.end()) { chunk = iter->second; }
	else
	{
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk)
		{
			SDL_Log("[AudioSystem] Failed to load sound file %s",
			        fileName.c_str());
			return nullptr;
		}

		mSounds.emplace(fileName, chunk);
	}
	return chunk;
}

// Input for debugging purposes
void AudioSystem::ProcessInput(const Uint8 *keyState)
{
	// Debugging code that outputs all active sounds on leading edge of period key
	if (keyState[SDL_SCANCODE_PERIOD] && !mLastDebugKey)
	{
		SDL_Log("[AudioSystem] Active Sounds:");
		for (size_t i = 0; i < mChannels.size(); i++)
		{
			if (mChannels[i].IsValid())
			{
				if (auto iter = mHandleMap.find(mChannels[i]);
					iter != mHandleMap.end())
				{
					HandleInfo &hi = iter->second;
					SDL_Log("Channel %d: %s, %s, looping = %d, paused = %d",
					        static_cast<unsigned>(i),
					        mChannels[i].GetDebugStr(),
					        hi.mSoundName.c_str(), hi.mIsLooping, hi.mIsPaused);
				}
				else
				{
					SDL_Log("Channel %d: %s INVALID", static_cast<unsigned>(i),
					        mChannels[i].GetDebugStr());
				}
			}
		}
	}

	mLastDebugKey = keyState[SDL_SCANCODE_PERIOD];
}
