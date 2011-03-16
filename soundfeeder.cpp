#include <cstdlib>

#include "soundfeeder.h"
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
        QMutex &m = Radio::self()->dataMutex();
        m.lock();
        while (!Radio::self()->hasChunk() && !Radio::self()->isExiting()) {
            Radio::self()->pcmWaitCondition().wait(&m);
        }
		
        if (Radio::self()->isExiting()) {
          m.unlock();
          break;
        }

        Chunk c = Radio::self()->nextChunk();
        if (c.m_dataFrames == -1) {
            emit pcmWritten(c);
            continue;
        }
        m.unlock();
        QMutex &m2 = Radio::self()->pcmMutex();
        m2.lock();
        while (!Radio::self()->isPlaying()) {
			
            Radio::self()->playCondition().wait(&m2);
			
        }
        Radio::self()->pcmHandle()->play(c);
        m2.unlock();
        free(c.m_data);
        if (Radio::self()->isPlaying()) {
            emit pcmWritten(c);
        }
        usleep(10000);
    }
}
