#include <cstdlib>

#include "soundfeeder.h"
#include "spotfmapp.h"
#include "radio.h"

SoundFeeder::SoundFeeder(QObject *parent)
    : QThread(parent)
{
}

SoundFeeder::~SoundFeeder()
{
}

void SoundFeeder::run()
{
    Q_FOREVER {
        Radio *radio = SpotFm::app()->radio();
        QMutex &m = radio->dataMutex();
        m.lock();
        while (!radio->hasChunk() && !radio->isExiting()) {
            radio->pcmWaitCondition().wait(&m);
        }
		
        if (radio->isExiting()) {
          m.unlock();
          break;
        }

        Chunk c = radio->nextChunk();
        if (c.m_dataFrames == -1) {
            emit pcmWritten(c);
            continue;
        }
        m.unlock();
        QMutex &m2 = radio->pcmMutex();
        m2.lock();
        while (!radio->isPlaying() && !radio->isExiting()) {
            radio->playCondition().wait(&m2);
        }
        radio->pcmHandle()->play(c);
        m2.unlock();
        free(c.m_data);
        if (radio->isPlaying()) {
            emit pcmWritten(c);
        }
    }
}
