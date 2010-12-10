/*
 * This file is part of Spokify.
 * Copyright (C) 2010 Rafael Fernández López <ereslibre@kde.org>
 *
 * Spokify is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Spokify is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Spokify.  If not, see <http://www.gnu.org/licenses/>.
 */

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
        m.unlock();
        QMutex &m2 = Radio::self()->pcmMutex();
        m2.lock();
        while (!Radio::self()->isPlaying()) {
            Radio::self()->playCondition().wait(&m2);
        }

        const int written = snd_pcm_writei(Radio::self()->pcmHandle(), c.m_data, c.m_dataFrames);
        if (written < 0) {
            snd_pcm_recover(Radio::self()->pcmHandle(), written, 1);
        }
        m2.unlock();
        free(c.m_data);
        if (Radio::self()->isPlaying()) {
            emit pcmWritten(c);
        }
        usleep(10000);
    }
}
