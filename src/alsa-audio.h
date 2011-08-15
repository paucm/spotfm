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
		
        ~AlsaAudio() { snd_pcm_close(m_snd); }
	
        void init();

        void prepare();
        void play(Chunk &chunk);
        float volume() { return 0; }
		
    public slots:
        void setVolume(int) {}
        void clear();

    private:
        snd_pcm_t *m_snd;
};

#endif
