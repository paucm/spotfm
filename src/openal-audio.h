#ifndef OPENAL_AUDIO_H
#define OPENAL_AUDIO_H

#include "audio.h"
#include <AL/al.h>
#include <AL/alc.h>

#define NUM_BUFFERS 3

class OpenalAudio : public Audio
{
    public:
        OpenalAudio();
        ~OpenalAudio();
		
        void init();
        void clear();
        void play(Chunk &chunk);
        void prepare();
		
    private:
        ALCdevice *m_device;
        ALCcontext *m_context;	
        ALuint m_buffer[NUM_BUFFERS];
        ALuint m_source;
		
        int m_frame;
};

#endif

