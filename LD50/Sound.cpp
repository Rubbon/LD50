#include "Sound.h"
#include "sndfile.h"
#include <iostream>
#include "AL/alext.h"


ALCdevice* Sound::soundDevice;
ALCcontext* Sound::soundContext;

float Sound::MASTER_GAIN_SFX = 1.0f;

std::vector<ALint> Sound::lsSoundEffects;
//std::vector<ALint> Sound::lsEfx;
std::vector<ALint> Sound::lsEfxSlots;
//std::vector<AudioSource*> Sound::lsAudioSources;
std::vector<AudioSource*> Sound::lsTempAudioSources;


// Effect object functions
static LPALGENEFFECTS alGenEffects;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALEFFECTI alEffecti;
static LPALEFFECTIV alEffectiv;
static LPALEFFECTF alEffectf;
static LPALEFFECTFV alEffectfv;
static LPALGETEFFECTI alGetEffecti;
static LPALGETEFFECTIV alGetEffectiv;
static LPALGETEFFECTF alGetEffectf;
static LPALGETEFFECTFV alGetEffectfv;

// Auxiliary Effect Slot object functions
static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;



void Sound::InitSoundDevice() {
	soundDevice = alcOpenDevice(NULL); // open default device
	if (!soundDevice) std::puts("SND: [ERROR] Couldn't open default sound device!\n");

	soundContext = alcCreateContext(soundDevice, NULL);
	if (!soundDevice) std::puts("SND: [ERROR] Couldn't create sound device context!\n");

	alcMakeContextCurrent(soundContext);

	const ALCchar* name = NULL;
	if (alcIsExtensionPresent(soundDevice, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(soundDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(soundDevice) != AL_NO_ERROR)
		name = alcGetString(soundDevice, ALC_DEVICE_SPECIFIER);
	printf("SND: Opened \"%s\"\n", name);

	//check if efx can be used
	if (!alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()), "ALC_EXT_EFX")) {
		fprintf(stderr, "SND: [ERROR] EFX not supported\n");
	}
	else {
		puts("SND: EFX is supported!");
	}

	//macro to help load the function pointers
#define LOAD_PROC(T, x)  ((x) = (T)alGetProcAddress(#x))
	LOAD_PROC(LPALGENEFFECTS, alGenEffects);
	LOAD_PROC(LPALDELETEEFFECTS, alDeleteEffects);
	LOAD_PROC(LPALISEFFECT, alIsEffect);
	LOAD_PROC(LPALEFFECTI, alEffecti);
	LOAD_PROC(LPALEFFECTIV, alEffectiv);
	LOAD_PROC(LPALEFFECTF, alEffectf);
	LOAD_PROC(LPALEFFECTFV, alEffectfv);
	LOAD_PROC(LPALGETEFFECTI, alGetEffecti);
	LOAD_PROC(LPALGETEFFECTIV, alGetEffectiv);
	LOAD_PROC(LPALGETEFFECTF, alGetEffectf);
	LOAD_PROC(LPALGETEFFECTFV, alGetEffectfv);

	LOAD_PROC(LPALGENAUXILIARYEFFECTSLOTS, alGenAuxiliaryEffectSlots);
	LOAD_PROC(LPALDELETEAUXILIARYEFFECTSLOTS, alDeleteAuxiliaryEffectSlots);
	LOAD_PROC(LPALISAUXILIARYEFFECTSLOT, alIsAuxiliaryEffectSlot);
	LOAD_PROC(LPALAUXILIARYEFFECTSLOTI, alAuxiliaryEffectSloti);
	LOAD_PROC(LPALAUXILIARYEFFECTSLOTIV, alAuxiliaryEffectSlotiv);
	LOAD_PROC(LPALAUXILIARYEFFECTSLOTF, alAuxiliaryEffectSlotf);
	LOAD_PROC(LPALAUXILIARYEFFECTSLOTFV, alAuxiliaryEffectSlotfv);
	LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTI, alGetAuxiliaryEffectSloti);
	LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTIV, alGetAuxiliaryEffectSlotiv);
	LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTF, alGetAuxiliaryEffectSlotf);
	LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTFV, alGetAuxiliaryEffectSlotfv);
#undef LOAD_PROC

}

void Sound::CleanupSoundDevice() {
	//unload soounds
	for (int i = 0; i < lsSoundEffects.size(); i++) Sound::UnloadSound(lsSoundEffects[i]);
	lsSoundEffects.clear();
	//unload efx
	//for (int i = 0; i < lsEfx.size(); i++) Sound::UnloadEffect(lsEfx[i]);
	//lsEfx.clear();
	//unload efx
	for (int i = 0; i < lsEfxSlots.size(); i++) Sound::UnloadEffectSlot(lsEfxSlots[i]);
	lsEfxSlots.clear();

	//reminder - audio sources don't need to be deleted per-se since their delete functions are automatically run on close

	std::cout << "SND: Unloaded SFX!" << std::endl;
	//remove sound device
	alcMakeContextCurrent(NULL);
	alcDestroyContext(soundContext);
	alcCloseDevice(soundDevice);
	std::cout << "SND: Closed Sound Device!" << std::endl;
}

void Sound::LoadGameAudio() {
	//BGM
	//LoadSound("res/snd/bgm_example.ogg");
	LoadSound("res/snd/invasion.ogg");
	LoadSound("res/snd/bobandgang.ogg");
	//SFX
	//LoadSound("res/snd/snd_example.ogg");
	LoadSound("res/snd/bullet.ogg");
	LoadSound("res/snd/bang.ogg");
	LoadSound("res/snd/broke.ogg");
	LoadSound("res/snd/plonk.ogg");
	LoadSound("res/snd/laser.ogg");
	LoadSound("res/snd/takeoff.ogg");
	LoadSound("res/snd/landjet.ogg");

	//efx
	EFXEAXREVERBPROPERTIES reverb;
	reverb = EFX_REVERB_PRESET_UNDERWATER;
	LoadEffect(&reverb);
}


//-----------------------------------------------------------------------------------------------------------------------------------------//
// SOUND DATA FUNCTIONS
//---------------------------------------------------------------------------------------------------------------------------------------//
//sound loading code mostly stolen from openAL examples
ALuint Sound::LoadSound(const char* filename) {
	ALenum err, format;
	ALuint buffer;
	SNDFILE* sndfile;
	SF_INFO sfinfo;
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;

	/* Open the audio file and check that it's usable. */
	sndfile = sf_open(filename, SFM_READ, &sfinfo);
	if (!sndfile) {
		fprintf(stderr, "SND: [ERROR] Could not open audio in %s: %s\n", filename, sf_strerror(sndfile));
		return 0;
	}
	if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
		sf_close(sndfile);
		return 0;
	}

	/* Get the sound format, and figure out the OpenAL format */
	// removed formats with more than 2 channels because they cause problems and will probably be unused
	format = AL_NONE;
	if (sfinfo.channels == 1) format = AL_FORMAT_MONO16;
	else if (sfinfo.channels == 2) format = AL_FORMAT_STEREO16;
	else if (sfinfo.channels == 3) {
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) format = AL_FORMAT_BFORMAT2D_16;
	}
	else if (sfinfo.channels == 4) {
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) format = AL_FORMAT_BFORMAT3D_16;
	}
	if (!format) {
		fprintf(stderr, "SND: [ERROR] Unsupported channel count: %d\n", sfinfo.channels);
		sf_close(sndfile);
		return 0;
	}

	/* Decode the whole audio file to a buffer. */
	membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

	num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if (num_frames < 1) {
		free(membuf);
		sf_close(sndfile);
		return 0;
	}
	num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

	/* Buffer the audio data into a new buffer object, then free the data and
	 * close the file.
	 */
	buffer = 0;
	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

	free(membuf);
	sf_close(sndfile);

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if (err != AL_NO_ERROR) {
		fprintf(stderr, "SND: [ERROR] OpenAL Error: %s\n", alGetString(err));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return 0;
	}

	std::cout << "SND: Loaded Sound: " << filename << "  - ID: " << buffer << std::endl;

	lsSoundEffects.push_back(buffer);
	return buffer;
}


void Sound::UnloadSound(ALuint soundID) {
	alDeleteBuffers(1, &soundID);
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
// EFFECT DATA FUNCTIONS
//---------------------------------------------------------------------------------------------------------------------------------------//
//taken from openAL efx example
ALuint Sound::LoadEffect(const EFXEAXREVERBPROPERTIES* reverb) {
	ALuint effect = 0;
	ALenum err;

	/* Create the effect object and check if we can do EAX reverb. */
	alGenEffects(1, &effect);
	if (alGetEnumValue("AL_EFFECT_EAXREVERB") != 0) {
		printf("SND: Using EAX Reverb\n");

		/* EAX Reverb is available. Set the EAX effect type then load the
		 * reverb properties. */
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);

		alEffectf(effect, AL_EAXREVERB_DENSITY, reverb->flDensity);
		alEffectf(effect, AL_EAXREVERB_DIFFUSION, reverb->flDiffusion);
		alEffectf(effect, AL_EAXREVERB_GAIN, reverb->flGain);
		alEffectf(effect, AL_EAXREVERB_GAINHF, reverb->flGainHF);
		alEffectf(effect, AL_EAXREVERB_GAINLF, reverb->flGainLF);
		alEffectf(effect, AL_EAXREVERB_DECAY_TIME, reverb->flDecayTime);
		alEffectf(effect, AL_EAXREVERB_DECAY_HFRATIO, reverb->flDecayHFRatio);
		alEffectf(effect, AL_EAXREVERB_DECAY_LFRATIO, reverb->flDecayLFRatio);
		alEffectf(effect, AL_EAXREVERB_REFLECTIONS_GAIN, reverb->flReflectionsGain);
		alEffectf(effect, AL_EAXREVERB_REFLECTIONS_DELAY, reverb->flReflectionsDelay);
		alEffectfv(effect, AL_EAXREVERB_REFLECTIONS_PAN, reverb->flReflectionsPan);
		alEffectf(effect, AL_EAXREVERB_LATE_REVERB_GAIN, reverb->flLateReverbGain);
		alEffectf(effect, AL_EAXREVERB_LATE_REVERB_DELAY, reverb->flLateReverbDelay);
		alEffectfv(effect, AL_EAXREVERB_LATE_REVERB_PAN, reverb->flLateReverbPan);
		alEffectf(effect, AL_EAXREVERB_ECHO_TIME, reverb->flEchoTime);
		alEffectf(effect, AL_EAXREVERB_ECHO_DEPTH, reverb->flEchoDepth);
		alEffectf(effect, AL_EAXREVERB_MODULATION_TIME, reverb->flModulationTime);
		alEffectf(effect, AL_EAXREVERB_MODULATION_DEPTH, reverb->flModulationDepth);
		alEffectf(effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, reverb->flAirAbsorptionGainHF);
		alEffectf(effect, AL_EAXREVERB_HFREFERENCE, reverb->flHFReference);
		alEffectf(effect, AL_EAXREVERB_LFREFERENCE, reverb->flLFReference);
		alEffectf(effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, reverb->flRoomRolloffFactor);
		alEffecti(effect, AL_EAXREVERB_DECAY_HFLIMIT, reverb->iDecayHFLimit);
	}
	else {
		printf("SND: Using Standard Reverb\n");

		/* No EAX Reverb. Set the standard reverb effect type then load the
		 * available reverb properties. */
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);

		alEffectf(effect, AL_REVERB_DENSITY, reverb->flDensity);
		alEffectf(effect, AL_REVERB_DIFFUSION, reverb->flDiffusion);
		alEffectf(effect, AL_REVERB_GAIN, reverb->flGain);
		alEffectf(effect, AL_REVERB_GAINHF, reverb->flGainHF);
		alEffectf(effect, AL_REVERB_DECAY_TIME, reverb->flDecayTime);
		alEffectf(effect, AL_REVERB_DECAY_HFRATIO, reverb->flDecayHFRatio);
		alEffectf(effect, AL_REVERB_REFLECTIONS_GAIN, reverb->flReflectionsGain);
		alEffectf(effect, AL_REVERB_REFLECTIONS_DELAY, reverb->flReflectionsDelay);
		alEffectf(effect, AL_REVERB_LATE_REVERB_GAIN, reverb->flLateReverbGain);
		alEffectf(effect, AL_REVERB_LATE_REVERB_DELAY, reverb->flLateReverbDelay);
		alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF, reverb->flAirAbsorptionGainHF);
		alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb->flRoomRolloffFactor);
		alEffecti(effect, AL_REVERB_DECAY_HFLIMIT, reverb->iDecayHFLimit);
	}

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if (err != AL_NO_ERROR) {
		fprintf(stderr, "SND: [ERROR] OpenAL error: %s\n", alGetString(err));
		if (alIsEffect(effect))
			alDeleteEffects(1, &effect);
		return 0;
	}

	std::cout << "SND: Loaded Effect - ID: " << effect << std::endl;
	ALuint slot;
	slot = 0;
	alGenAuxiliaryEffectSlots(1, &slot);
	alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, (ALint)effect);
	lsEfxSlots.push_back(slot);
	alDeleteEffects(1, &effect);
	std::cout << "SND: Generated Effect Slot - ID: " << slot << std::endl;
	return slot;

	//lsEfx.push_back(effect);
	//return effect;
}
/*
void Sound::UnloadEffect(ALuint effectID) {
	alDeleteEffects(1, &effectID);
}


ALuint Sound::GenEffectSlot(ALint effectID) {
	ALuint slot;
	slot = 0;
	alGenAuxiliaryEffectSlots(1, &slot);
	alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, (ALint)effectID);
	std::cout << "SND: Generated Effect Slot - ID: " << slot << std::endl;
	return slot;
}
*/
void Sound::UnloadEffectSlot(ALuint effectID) {
	alDeleteAuxiliaryEffectSlots(1, &effectID);
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
// SOUND FUNCTIONS FOR GAMEPLAY
//---------------------------------------------------------------------------------------------------------------------------------------//
void Sound::Tick() {
	//loop through audio sources that auto-destroy on sound finished
	if (lsTempAudioSources.size() > 0) {
		ALint _soundInfo;
		for (int i = 0; i < lsTempAudioSources.size(); i++) {
			alGetSourcei(lsTempAudioSources[i]->id, AL_SOURCE_STATE, &_soundInfo);
			if (_soundInfo == AL_STOPPED) lsTempAudioSources[i]->Delete();
		}
	}
}

int Sound::PlayTempSoundAt(ALuint sound, float x, float y, float gain, float pitch) {
	AudioSource* _snd = new AudioSource();
	_snd->Generate(true);
	_snd->SetPos(x / 24, y / 24);
	_snd->Play(sound, false, true);
	_snd->SetGain(gain);
	_snd->SetPitch(pitch);
	return 0;// lsTempAudioSources.size() - 1;
}

int Sound::PlayTempSound(ALuint sound, float gain, float pitch) {
	AudioSource* _snd = new AudioSource();
	_snd->Generate(true);
	_snd->Play(sound, false, true);
	_snd->SetGain(gain);
	_snd->SetPitch(pitch);
	return 0;// lsTempAudioSources.size() - 1;
}

void Sound::SetListenerPos(float x, float y) {
	alListener3f(AL_POSITION, x / 24, 0, y / 24);
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
// AUDIO SOURCE
//---------------------------------------------------------------------------------------------------------------------------------------//
/*
AudioSource::AudioSource(ALint soundId, bool loop, bool destroyAtEnd) {
	alGenSources(1, &id);
	soundBufferID = soundId;
	alSourcei(id, AL_BUFFER, (ALint)soundBufferID);
	alSourcef(id, AL_PITCH, 1.0f);
	alSourcef(id, AL_GAIN, 1.0f);
	alSource3f(id, AL_POSITION, 0, 0, 0);
	//alSource3f(id, AL_VELOCITY, 0, 0, 0);
	alSourcei(id, AL_LOOPING, loop);
	destroyAfterDone = destroyAtEnd;
	if (destroyAfterDone == true) {
		Sound::lsTempAudioSources.push_back(this);
	}

	std::cout << "SND: Generated new AudioSource, ID: " << id << std::endl;
}

AudioSource::AudioSource() {
	alGenSources(1, &id);
	soundBufferID = 0;
	alSourcei(id, AL_BUFFER, (ALint)soundBufferID);
	alSourcef(id, AL_PITCH, 1.0f);
	alSourcef(id, AL_GAIN, 1.0f);
	alSource3f(id, AL_POSITION, 0, 0, 0);
	//alSource3f(id, AL_VELOCITY, 0, 0, 0);
	alSourcei(id, AL_LOOPING, false);
	std::cout << "SND: Generated new AudioSource, ID: " << id << std::endl;
}
*/

void AudioSource::Generate(bool destroyAtEnd) {
	alGenSources(1, &id);
	//soundBufferID = soundId;
	alSourcei(id, AL_BUFFER, (ALint)soundBufferID);
	alSourcef(id, AL_PITCH, 1.0f);
	alSourcef(id, AL_GAIN, gain);
	alSource3f(id, AL_POSITION, x, 0, y);
	//alSource3f(id, AL_VELOCITY, 0, 0, 0);
	alSourcei(id, AL_LOOPING, AL_FALSE);
	destroyAfterDone = destroyAtEnd;
	if (destroyAfterDone == true) {
		Sound::lsTempAudioSources.push_back(this);
	}

	deleted = false;
	std::cout << "SND: Generated new AudioSource, ID: " << id << std::endl;
}

void AudioSource::Delete() {
	if (!deleted) {
		std::cout << "SND: AudioSource self delete, ID: " << id << std::endl;

		alDeleteSources(1, &id);

		if (destroyAfterDone) {
			Sound::lsTempAudioSources.erase(std::find(Sound::lsTempAudioSources.begin(), Sound::lsTempAudioSources.end(), this));
			deleted = true;
			delete this;
		}
		deleted = true;
	}
}

void AudioSource::SetPos(float _x, float _y) {
	alSource3f(id, AL_POSITION, _x, 0, _y);
	x = _x;
	y = _y;
}

void AudioSource::SetPitch(float val) {
	alSourcef(id, AL_PITCH, val);
}

void AudioSource::SetGain(float val) {
	gain = val;
	alSourcef(id, AL_GAIN, val);
}

void AudioSource::Play(ALint soundID, bool loop, bool destroyAtEnd) {
	//if (soundID != soundBufferID) {
	soundBufferID = soundID;
	alSourcei(id, AL_BUFFER, (ALint)soundBufferID);
	//}
	alSourcei(id, AL_LOOPING, loop);
	alSourcePlay(id);
	destroyAfterDone = destroyAtEnd;
}


bool AudioSource::IsPlaying() {
	ALint _soundInfo;
	alGetSourcei(id, AL_SOURCE_STATE, &_soundInfo);
	if (_soundInfo == AL_STOPPED) return false;
	return true;
}

void AudioSource::Stop() {
	alSourceStop(id);
}

void AudioSource::SetLooping(bool shouldLoop) {
	alSourcei(id, AL_LOOPING, shouldLoop);
}

void AudioSource::SetEffect(ALint effectSlot, short send, short filter) {
	alSource3i(id, (ALint)AL_AUXILIARY_SEND_FILTER, effectSlot, send, AL_FILTER_NULL);
}

void AudioSource::SetSoundEffect(ALint soundID) {
	soundBufferID = soundID;
	alSourcei(id, AL_BUFFER, (ALint)soundBufferID);
}