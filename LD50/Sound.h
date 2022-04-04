#pragma once
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/efx-presets.h"
#include "AL/efx.h"
#include <vector>

//todo - function to change audiosources gain over time?
struct AudioSource {
	ALuint id = 0;
	ALint soundBufferID = 0;
	bool destroyAfterDone = false;
	bool deleted = true;
	ALfloat gain = 1.0f;
	float x = 0, y = 0;

	// soundID 0 = doesn't play a sound on create
	//AudioSource(ALint soundId, bool loop, bool destroyAtEnd);
	//AudioSource();
	void Generate(bool destroyAtEnd);
	~AudioSource() { Delete(); };
	void Delete();

	void SetPos(float _x, float _y);
	// range 0.5 - 2.0
	void SetPitch(float val);
	void SetSoundEffect(ALint soundID);
	void SetGain(float val);
	void Play(ALint soundID, bool loop, bool destroyAtEnd);
	bool IsPlaying();
	void Stop();
	void SetLooping(bool shouldLoop);
	void SetEffect(ALint effectSlot, short send, short filter = AL_FILTER_NULL);
};


class Sound {
public:
	static float MASTER_GAIN_SFX; //todo - unused rn

	static std::vector<ALint> lsSoundEffects;
	//static std::vector<ALint> lsEfx;
	static std::vector<ALint> lsEfxSlots;
	//static std::vector<AudioSource*> lsAudioSources;
	static std::vector<AudioSource*> lsTempAudioSources;


	static void InitSoundDevice();
	static void CleanupSoundDevice();
	static void LoadGameAudio();
	static void Tick();
	//Sound playing functions
	//returns the pos in the list of temp sounds it's at, for if you want to do additional sounds
	static int PlayTempSoundAt(ALuint sound, float x, float y, float gain = 1.0f, float pitch = 1.0f);
	static void SetListenerPos(float x, float y);

	//audio file loading
	static ALuint LoadSound(const char* filename);
	static void UnloadSound(ALuint soundID);

	//audio effect stuff
	static ALuint LoadEffect(const EFXEAXREVERBPROPERTIES* reverb);
	//static void UnloadEffect(ALuint effectID);

	//static ALuint GenEffectSlot(ALint effectID);
	static void UnloadEffectSlot(ALuint effectID);


private:
	static ALCdevice* soundDevice;
	static ALCcontext* soundContext;

};

enum Sfx {
	SFX_NOSOUND,

	//MUSIC
	//BGM_EXAMPLE,
	BGM_INVASION,
	//SOUNDS
	//SND_EXAMPLE,
	SND_BULLET,

};

enum Efx {
	FX_NONE,
	FX_THROUGH_WALL,

};