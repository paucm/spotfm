#ifndef OPENAL_AUDIO_H
#define OPENAL_AUDIO_H

#include "audio.h"
#include <AL/al.h>
#include <AL/alc.h>

#define NUM_BUFFERS 5

class OpenalAudio : public Audio
{
    public:
        OpenalAudio();
        ~OpenalAudio();

        void init();
        void play(Chunk &chunk);
        void prepare();
        float volume() { return m_volume; }

    public slots:
        void setVolume(int volume);
        void clear();

    private:
        ALCdevice *m_device;
        ALCcontext *m_context;
        ALuint m_buffer[NUM_BUFFERS];
        ALuint m_source;

        int m_frame;
        float m_volume;
};

#endif

