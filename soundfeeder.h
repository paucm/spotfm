#ifndef SOUND_FEEDER_H
#define SOUND_FEEDER_H

#include <QtCore/QThread>

#include "chunk.h"

class SoundFeeder
    : public QThread
{
    Q_OBJECT

public:
    SoundFeeder(QObject *parent = 0);
    virtual ~SoundFeeder();
    
  signals:
    void pcmWritten(const Chunk &chunk);
    
protected:
    virtual void run();
};

#endif

