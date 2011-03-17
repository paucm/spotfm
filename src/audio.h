#ifndef AUDIO_H
#define AUDIO_H

#include "chunk.h"

class Audio
{
    public:
	    virtual void init() = 0;
        virtual void clear() = 0;
        virtual void play(Chunk &chunk) = 0;
        virtual void prepare() = 0;
};

#endif
