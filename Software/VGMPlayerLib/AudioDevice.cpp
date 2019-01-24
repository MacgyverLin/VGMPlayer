#include "AudioDevice.h"
#ifdef STM32
#include <Array.h>
#include <stdlib.h>
#include <al.h>
#else
#include <Array.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#endif
using namespace std;

#ifdef STM32

u32 AlsourceId = 0;
class ALsource
{
public:
	ALsource()
	{
		uid = ++AlsourceId;
	}

	~ALsource()
	{
	}
	
	void play()
	{
	}
	
	void stop()
	{
	}

	void rewind()
	{
	}

	void pause()
	{
	}
	
	void queueBuffers(ALsizei nb, const ALuint *buffers)
	{
	}

	ALvoid setf(ALenum param, ALfloat value)
	{
	}

	ALvoid set3f(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
	{
	}

	ALvoid setfv(ALenum param, const ALfloat *values)
	{
	}

	ALvoid seti(ALenum param, ALint value)
	{
	}

	ALvoid set3i(ALenum param, ALint value1, ALint value2, ALint value3)
	{
	}

	ALvoid setiv(ALenum param, const ALint *values)
	{
	}

	ALvoid getf(ALenum param, ALfloat *value)
	{
	}
	
	ALvoid get3f(ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3)
	{
	}

	ALvoid getfv(ALenum param, ALfloat *values)
	{
	}

	ALvoid geti(ALenum param, ALint *value)
	{
	}

	ALvoid get3i(ALenum param, ALint *value1, ALint *value2, ALint *value3)
	{
	}

	ALvoid getiv(ALenum param, ALint *values)
	{
	}
	
	u32 uid;
	Vector<u8> buf;
};

u32 ALbufferId = 0;
class ALbuffer
{
public:
	ALbuffer()
	{
		uid = ++ALbufferId;
	}
	
	~ALbuffer()
	{
	}	
	
	ALvoid setData(ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
	{
	}

	u32 uid;
	Vector<u8> buf;
};

class ALCcontext
{
public:
	ALCcontext()
	{
		speedOfSound = 0;
		dopplerVelocity = 0;
		dopplerFactor = 0;
		lastSource = 0;
		lastBuffer = 0;
	}
	
	~ALCcontext()
	{
		if(current==this)
			current = 0;
		
		for(u32 i=0; i<sources.size(); i++)
		{
			delete sources[i];
			sources[i] = 0;			
		}
		
		for(u32 i=0; i<buffers.size(); i++)
		{
			delete buffers[i];
			buffers[i] = 0;
		}
		
		speedOfSound = 0;
		dopplerVelocity = 0;
		dopplerFactor = 0;
		lastSource = 0;
		lastBuffer = 0;		
	}
	
	boolean makeCurrent()
	{
		current = this;
		
		return TRUE;
	}	
	
	void setSpeedOfSound(f32 value)
	{
		speedOfSound = value;
	}
	
	void setDopplerVelocity(f32 value)
	{
		dopplerVelocity = value;
	}
	
	void setDopplerFactor(f32 value)
	{
		dopplerFactor = value;
	}	
	
	f32 getSpeedOfSound()
	{
		return speedOfSound;
	}
	
	f32 getDopplerVelocity()
	{
		return dopplerVelocity;
	}
	
	f32 getDopplerFactor()
	{
		return dopplerFactor;
	}
	
	ALvoid genSources(u32 n, u32 *srcs)
	{
		for(u32 i=0; i<n; i++)
		{
			ALsource* src = new ALsource();

			srcs[i] = src->uid;
			sources.push() = src;
		}
	}

	ALvoid deleteSources(u32 n, const u32 *srcs)
	{
	}
	
	ALboolean isSource(u32 source)
	{
		for(u32 i=0; i<sources.size(); i++)
		{
			if(sources[i]->uid == source)
				return TRUE;
		}

		return FALSE;
	}
	
	ALvoid genBuffers(u32 n, u32 *bufs)
	{
		for(u32 i=0; i<n; i++)
		{
			ALbuffer* buf = new ALbuffer();

			bufs[i] = buf->uid;
			buffers.push() = buf;
		}
	}

	ALvoid deleteBuffers(u32 n, const u32 *buffers)
	{
	}

	ALboolean isBuffer(u32 buffer)
	{
		for(u32 i=0; i<buffers.size(); i++)
		{
			if(buffers[i]->uid == buffer)
				return TRUE;
		}

		return FALSE;
	}
	
	ALvoid sourcePlay(u32 source)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->play();
		}
	}

	ALvoid sourceStop(u32 source)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->stop();
		}
	}

	ALvoid sourceRewind(u32 source)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->rewind();
		}
	}

	ALvoid sourcePause(u32 source)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->pause();
		}
	}
	
	ALvoid sourceQueueBuffers(ALuint source, ALsizei nb, const ALuint *buffers)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->queueBuffers(nb, buffers);
		}
	}
	
	ALvoid sourcef(ALuint source, ALenum param, ALfloat value)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->setf(param, value);
		}		
	}

	ALvoid source3f(ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->set3f(param, value1, value2, value3);
		}		
	}

	ALvoid sourcefv(ALuint source, ALenum param, const ALfloat *values)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->setfv(param, values);
		}		
	}

	ALvoid sourcei(ALuint source, ALenum param, ALint value)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->seti(param, value);
		}		
	}

	ALvoid source3i(ALuint source, ALenum param, ALint value1, ALint value2, ALint value3)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->set3i(param, value1, value2, value3);
		}		
	}

	ALvoid sourceiv(ALuint source, ALenum param, const ALint *values)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->setiv(param, values);
		}
	}
	
	ALvoid getSourcef(ALuint source, ALenum param, ALfloat *value)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->getf(param, value);
		}		
	}
	
	ALvoid getSource3f(ALuint source, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->get3f(param, value1, value2, value3);
		}
	}

	ALvoid getSourcefv(ALuint source, ALenum param, ALfloat *values)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->getfv(param, values);
		}
	}

	ALvoid getSourcei(ALuint source,  ALenum param, ALint *value)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->geti(param, value);
		}		
	}

	ALvoid getSource3i(ALuint source, ALenum param, ALint *value1, ALint *value2, ALint *value3)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->get3i(param, value1, value2, value3);
		}
	}

	ALvoid getSourceiv(ALuint source, ALenum param, ALint *values)
	{
		ALsource* src = findSource(source);
		if(src)
		{
			src->getiv(param, values);
		}
	}
		
	ALvoid bufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
	{
		ALbuffer* buf = findBuffer(buffer);
		if(buf)
		{
			buf->setData(format, data, size, freq);
		}		
	}
private:
	ALsource* findSource(u32 source)
	{
		if(lastSource && lastSource->uid==source)
			return lastSource;

		for(u32 i=0; i<sources.size(); i++)
		{
			if(sources[i]->uid == source)
			{
				lastSource = sources[i];
				return sources[i];
			}
		}

		return 0;
	}

	ALbuffer* findBuffer(u32 buffer)
	{
		if(lastBuffer && lastBuffer->uid==buffer)
			return lastBuffer;
		
		for(u32 i=0; i<buffers.size(); i++)
		{
			if(buffers[i]->uid == buffer)
			{
				lastBuffer = buffers[i];
				return buffers[i];
			}
		}

		return 0;
	}
////////////////////////////////////////////////////////////
public:
protected:
private:
	f32 speedOfSound;
	f32 dopplerVelocity;
	f32 dopplerFactor;
	Vector<ALsource *> sources;
	Vector<ALbuffer *> buffers;

	ALsource *lastSource;
	ALbuffer *lastBuffer;
public:
	static ALCcontext *current;
protected:
private:
};

ALCcontext *ALCcontext::current = 0;

class ALCdevice
{
public:
	ALCdevice()
	{
	}

	~ALCdevice()
	{
	}
	
	ALCcontext* createContext(const s32* attrlist)
	{
		return new ALCcontext();
	}
	
	void alcDestroyContext(ALCcontext *context)
	{
		if(context)
		{
			delete context;
		}
	}
	
	static ALCdevice* openDevice(const s8 *devicename)
	{
		ALCdevice* device = new ALCdevice();
	
		return device;
	}

	static ALboolean closeDevice(ALCdevice *device)
	{
		if(device)
		{
			delete device;
			device = 0;
		}
	}	
protected:
private:
////////////////////////////////////////////////////////////
public:
protected:
private:	
};

ALCdevice* alcOpenDevice(const s8 *devicename)
{
	return ALCdevice::openDevice(devicename);
}

boolean alcCloseDevice(ALCdevice *device)
{
	return ALCdevice::closeDevice(device);
}

ALCcontext* alcCreateContext(ALCdevice *device, const ALint* attrlist)
{
	if(!device)
		return 0;
	
	return device->createContext(attrlist);
}

void alcDestroyContext(ALCcontext *context)
{
	if(context)
	{
		delete context;
	}
}

ALboolean alcMakeContextCurrent(ALCcontext *context)
{
	if(!context)
		return 0;
	
	return context->makeCurrent();
}

AL_API void AL_APIENTRY alSpeedOfSound(ALfloat value)
{
	ALCcontext::current->setSpeedOfSound(value);
}

AL_API void AL_APIENTRY alDopplerVelocity(ALfloat value)
{
	ALCcontext::current->setDopplerVelocity(value);
}

AL_API void AL_APIENTRY alDopplerFactor(ALfloat value)
{
	ALCcontext::current->setDopplerFactor(value);
}

AL_API void AL_APIENTRY alSourcef(ALuint source, ALenum param, ALfloat value)
{
	ALCcontext::current->sourcef(source, param, value);
}

AL_API void AL_APIENTRY alSource3f(ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
	ALCcontext::current->source3f(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alSourcefv(ALuint source, ALenum param, const ALfloat *values)
{
	ALCcontext::current->sourcefv(source, param, values);
}

AL_API void AL_APIENTRY alSourcei(ALuint source, ALenum param, ALint value)
{
	ALCcontext::current->sourcei(source, param, value);
}

AL_API void AL_APIENTRY alSource3i(ALuint source, ALenum param, ALint value1, ALint value2, ALint value3)
{
	ALCcontext::current->source3i(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alSourceiv(ALuint source, ALenum param, const ALint *values)
{
	ALCcontext::current->sourceiv(source, param, values);
}

AL_API void AL_APIENTRY alGetSourcef(ALuint source, ALenum param, ALfloat *value)
{
	ALCcontext::current->getSourcef(source, param, value);
}

AL_API void AL_APIENTRY alGetSource3f(ALuint source, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3)
{
	ALCcontext::current->getSource3f(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetSourcefv(ALuint source, ALenum param, ALfloat *values)
{
	ALCcontext::current->getSourcefv(source, param, values);
}

AL_API void AL_APIENTRY alGetSourcei(ALuint source,  ALenum param, ALint *value)
{
	ALCcontext::current->getSourcei(source, param, value);
}

AL_API void AL_APIENTRY alGetSource3i(ALuint source, ALenum param, ALint *value1, ALint *value2, ALint *value3)
{
	ALCcontext::current->getSource3i(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetSourceiv(ALuint source,  ALenum param, ALint *values)
{
	ALCcontext::current->getSourceiv(source, param, values);
}

AL_API void AL_APIENTRY alGenSources(ALsizei n, ALuint *sources)
{
	ALCcontext::current->genSources(n, sources);
}

AL_API void AL_APIENTRY alDeleteSources(ALsizei n, const ALuint *sources)
{
	ALCcontext::current->deleteSources(n, sources);
}

AL_API ALboolean AL_APIENTRY alIsSource(ALuint source)
{
	return ALCcontext::current->isSource(source);
}

AL_API void AL_APIENTRY alGenBuffers(ALsizei n, ALuint *buffers)
{
	ALCcontext::current->genBuffers(n, buffers);
}

AL_API void AL_APIENTRY alDeleteBuffers(ALsizei n, const ALuint *buffers)
{
	ALCcontext::current->deleteBuffers(n, buffers);
}

AL_API ALboolean AL_APIENTRY alIsBuffer(ALuint buffer)
{
	return ALCcontext::current->isBuffer(buffer);
}

AL_API void AL_APIENTRY alSourcePlay(ALuint source)
{
	ALCcontext::current->sourcePlay(source);
}

AL_API void AL_APIENTRY alSourceStop(ALuint source)
{
	ALCcontext::current->sourceStop(source);
}

AL_API void AL_APIENTRY alSourceRewind(ALuint source)
{
	ALCcontext::current->sourceRewind(source);
}

AL_API void AL_APIENTRY alSourcePause(ALuint source)
{
	ALCcontext::current->sourcePause(source);
}

AL_API void AL_APIENTRY alBufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
{
	ALCcontext::current->bufferData(buffer, format, data, size, freq);
}

AL_API void AL_APIENTRY alSourceQueueBuffers(ALuint source, ALsizei nb, const ALuint *buffers)
{
	ALCcontext::current->sourceQueueBuffers(source, nb, buffers);
}

AL_API ALenum AL_APIENTRY alGetError(void)
{
	return AL_NO_ERROR;
}

#endif

class AudioDeviceImpl
{
public:
	ALCcontext*		context;
	ALCdevice*		device;
	ALuint			outSource;
	ALint			processedBuffer;
	ALint			queuedBuffer;

	Vector<ALuint>	sndBuffers;

	s32			channels;
	s32			bitsPerSample;
	s32			sampleRate;

	f32			volume;
	f32			playRate;

	s32			WP;
};

AudioDevice::AudioDevice()
{
	impl = new AudioDeviceImpl();
}

AudioDevice::~AudioDevice()
{
	if (impl)
	{
		delete impl;
		impl = 0;
	}
}

boolean AudioDevice::open(s32 channels_, s32 bitsPerSample_, s32 sampleRate_, s32 bufferCount_)
{
	ALuint error = 0;

	impl->context = 0;
	impl->device = 0;
	impl->outSource = 0;
	impl->processedBuffer = 0;
	impl->queuedBuffer = 0;
	impl->channels = channels_;
	impl->bitsPerSample = bitsPerSample_;
	impl->sampleRate = sampleRate_;
	impl->playRate = 1.0;
	impl->volume = 1.0;

	impl->WP = 0;

	// device
	impl->device = alcOpenDevice(NULL);
	if (!impl->device)
		return false;

	// context
	impl->context = alcCreateContext(impl->device, NULL);
	alcMakeContextCurrent(impl->context);

	// sources
	alGenSources(1, &impl->outSource);
	alSpeedOfSound(1.0);
	alDopplerVelocity(1.0);
	alDopplerFactor(1.0);
	alSourcei(impl->outSource, AL_LOOPING, AL_FALSE);
	alSourcef(impl->outSource, AL_PITCH, impl->playRate);
	alSourcef(impl->outSource, AL_GAIN, impl->volume);

	// create 32 buffers
	impl->sndBuffers.resize(bufferCount_);
	alGenBuffers(impl->sndBuffers.size(), &impl->sndBuffers[0]);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		//printf("error alGenBuffers %x \n", error);
		return FALSE;
	}

	return TRUE;
}

void AudioDevice::close()
{
	if (impl->sndBuffers.size() != 0)
	{
		alDeleteBuffers(impl->sndBuffers.size(), &impl->sndBuffers[0]);
	}

	if(impl->outSource)
	{
		alDeleteSources(1, &impl->outSource);
		impl->outSource = 0;
	}

	if(impl->device)
	{
		alcCloseDevice(impl->device);
		impl->device = NULL;
	}

	if(impl->context)
	{
		alcMakeContextCurrent(NULL);

		alcDestroyContext(impl->context);
		impl->context = NULL;
	}
}

s32 AudioDevice::play()
{
	alSourcePlay(impl->outSource);

	ALuint error;
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		//printf("error playSound %x\n", error);
		return 0;
	}

	return -1;
}

s32 AudioDevice::stop()
{
	alSourceStop(impl->outSource);

	ALuint error;
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		//printf("error stopSound %x\n", error);
		return 0;
	}

	return -1;
}

s32 AudioDevice::getDeviceState()
{
	int sourceState;
	alGetSourcei(impl->outSource, AL_SOURCE_STATE, &sourceState);

	switch (sourceState)
	{
	case AL_INITIAL:
		return 0;
		break;
	case AL_PAUSED:
		return 1;
		break;
	case AL_STOPPED:
		return 2;
		break;
	default:
	case AL_PLAYING:
		return 3;
		break;
	};
}

s32 AudioDevice::update()
{
	int processed;
	alGetSourcei(impl->outSource, AL_BUFFERS_PROCESSED, &processed);
	alGetSourcei(impl->outSource, AL_BUFFERS_QUEUED, &impl->queuedBuffer);
	impl->processedBuffer += processed;
	//printf("processedBuffer: %d, processed: %d, queuedBuffer: %d\n", processedBuffer, processed, queuedBuffer);

	while (processed--)
	{
		ALuint buff;

		// unqueue process buffer
		alSourceUnqueueBuffers(impl->outSource, 1, &buff);

		ALuint error;
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			//printf("error alSourceUnqueueBuffers %x\n", error);
			//AL_ILLEGAL_ENUM
			//AL_INVALID_VALUE
			//#define AL_ILLEGAL_COMMAND                        0xA004
			//#define AL_INVALID_OPERATION                      0xA004
			return 0;
		}
	}

	return -1;
}

s32 AudioDevice::queue(void* data_, int dataSize_)
{
	ALenum format = 0;
	ALuint error = 0;

	if (impl->bitsPerSample == 8)
	{
		if (impl->channels == 1)
		{
			format = AL_FORMAT_MONO8;
		}
		else if (impl->channels == 2)
		{
			format = AL_FORMAT_STEREO8;
		}
		else
		{
			//printf("Error: Channel must be 1 or 2\n");
			return 0;
		}
	}
	else if (impl->bitsPerSample == 16)
	{
		if (impl->channels == 1)
		{
			format = AL_FORMAT_MONO16;
		}
		else if (impl->channels == 2)
		{
			format = AL_FORMAT_STEREO16;
		}
		else
		{
			//printf("Error: Channel must be 1 or 2\n");
			return 0;
		}
	}
	else
	{
		//printf("Error: bit per sample must be 8 or 16\n");
		return 0;
	}

	// fill data
	// printf("soundDevice->WP %d\n", WP);
	ALuint buffer = impl->sndBuffers[impl->WP];
	alBufferData(buffer, format, data_, dataSize_, impl->sampleRate);
	error = alGetError();
	int p=0, q = 0;
	if (error != AL_NO_ERROR)
	{
		alGetSourcei(impl->outSource, AL_BUFFERS_PROCESSED, &p);
		alGetSourcei(impl->outSource, AL_BUFFERS_QUEUED, &q);
		//printf("alGetError %x: WP=%d, processed: %d, queued: %d, alBufferData(%x, %x, %p, %d, %d)\n", error, impl->WP, p, q, buffer, format, data_, dataSize_, impl->sampleRate);
		//AL_ILLEGAL_ENUM
		//AL_INVALID_VALUE
		//#define AL_ILLEGAL_COMMAND                        0xA004
		//#define AL_INVALID_OPERATION                      0xA004
		return 0;
	}

	// queue a buffer
	alSourceQueueBuffers(impl->outSource, 1, &buffer);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		// printf("error alSourceQueueBuffers %x\n", error);
		return 0;
	}

	impl->WP = impl->WP + 1;
	if(impl->WP>= impl->sndBuffers.size())
	{
		impl->WP = 0;
	}
	//WP = (WP + 1) % sndBuffers.size();

	return -1;
}

s32 AudioDevice::getQueued()
{
	return impl->queuedBuffer;
}

void AudioDevice::setVolume(f32 volume_)
{
	impl->volume = volume_;

	alSourcef(impl->outSource, AL_GAIN, impl->volume);
}

f32 AudioDevice::getVolume()
{
	return impl->volume;
}

void AudioDevice::setPlayRate(f32 playRate_)
{
	impl->playRate = playRate_;

	alSourcef(impl->outSource, AL_PITCH, impl->playRate);
}

f32 AudioDevice::getPlayRate()
{
	return impl->playRate;
}
