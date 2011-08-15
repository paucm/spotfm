#ifndef AUDIO_H
#define AUDIO_H

#include "chunk.h"

class Audio
{
    public:
	    virtual void init() = 0;
        virtual void play(Chunk &chunk) = 0;
        virtual void prepare() = 0;
        virtual float volume() = 0;
    public slots:
        virtual void setVolume(int volume) = 0;
        virtual void clear() = 0;
};

#endif
