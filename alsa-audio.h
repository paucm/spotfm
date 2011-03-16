#ifndef ALSA_AUDIO_H
#define ALSA_AUDIO_H

#include <alsa/asoundlib.h>

#include "audio.h"

class AlsaAudio : public Audio
{
	public:
        AlsaAudio()
        {
        }
		
        ~AlsaAudio()
        {
        }
		
        void init();
        void clear();
        void prepare();
        void play(Chunk &chunk);
		
    private:
        snd_pcm_t *m_snd;
};

#endif
