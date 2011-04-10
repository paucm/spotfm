#include <unistd.h>
#include "openal-audio.h"

OpenalAudio::OpenalAudio()
{
    m_device = NULL;
    m_context = NULL;
    m_frame = 0;
}

OpenalAudio::~OpenalAudio()
{
    if(m_context)
        alcDestroyContext(m_context);

    if(m_device)
        alcCloseDevice(m_device);
}

void OpenalAudio::init()
{
    const ALCchar *default_device;
    default_device = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    m_device = alcOpenDevice(default_device); /* Use the default device */
    if (!m_device) {
        qDebug("Failed to open device");
        return;
    }
    m_context = alcCreateContext(m_device, NULL);
    alcMakeContextCurrent(m_context);
    alListenerf(AL_GAIN, 1.0f);
    alDistanceModel(AL_NONE);
}

void OpenalAudio::prepare()
{
    alGenBuffers(NUM_BUFFERS, m_buffer);
    alGenSources(1, &m_source);
    if(alGetError() != AL_NO_ERROR) {
        qDebug("Error generating :(");
        return;
    }
    m_frame = 0;
}

void OpenalAudio::clear()
{
    alSourceStop(m_source);
    for(int i = 0; i < NUM_BUFFERS; i++) {
        alSourceUnqueueBuffers(m_source, 1, &m_buffer[i]);
    }
    alDeleteBuffers(NUM_BUFFERS, m_buffer);
    alDeleteSources(1, &m_source);
}

void OpenalAudio::play(Chunk &chunk)
{
    ALint val;
    ALuint buf = m_buffer[m_frame % NUM_BUFFERS];

    alSourceUnqueueBuffers(m_source, 1, &buf);

    alBufferData(buf,
                 chunk.m_channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
                 chunk.m_data,
                 chunk.m_dataFrames * sizeof(int16_t) * chunk.m_channels,
                 chunk.m_rate);
    alSourceQueueBuffers(m_source, 1, &buf);
	
    //First, we need to prebuffer some audio
    if(m_frame < NUM_BUFFERS-1) {
        m_frame++;
        return;
    }
	
    alGetSourcei(m_source, AL_SOURCE_STATE, &val);
    if(val != AL_PLAYING) {
        alSourcePlay(m_source);
    }

    do {
        alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &val);
        usleep(10);
    } while (!val);
    m_frame++;
}
