#include <cstdlib>

#include "audiocontroller.h"

AudioController::AudioController(QObject *parent)
 : QThread(parent)
 , m_currentTrack()
 , m_trackPos(0)
 , m_state(Stopped)
{
}

AudioController::~AudioController()
{
}

bool AudioController::isPlaying() const
{
    return m_state == Playing;
}

void AudioController::play()
{
    loadNext();
    m_playCondition.wakeAll();
}

void AudioController::stop()
{
    m_state = Stopped;
    clearSoundQueue();
    SpotifySession::self()->unload();
    emit trackEnded(m_trackPos/1000);
    m_trackPos = 0;
}

void AudioController::pause()
{
    if (m_state == Playing)
        m_state = Paused;
}

void AudioController::unpause()
{
    if (m_state == Paused) {
        m_state = Playing;
        m_playCondition.wakeAll();
    }
}

void AudioController::loadNext()
{
    if(!m_playlist.isEmpty())
    {
        if(m_state != Stopped) stop();
        m_currentTrack = m_playlist.dequeue();
        if (!SpotifySession::self()->load(m_currentTrack)) {
            emit error(InvalidTrack, tr("The track cannot be played"));
            return;
        }
        prepare();
        m_state = Playing;
        emit trackStarted();
    }
    else {
        emit error(PlaylistEnded, tr("Sorry, threre is no more content available"));
        stop();
    }
}

void AudioController::enqueue(const Track &track)
{
    m_playlist.enqueue(track);
}

void AudioController::clearQueue()
{
    m_playlist.clear();
}

void AudioController::newChunk(const Chunk &chunk)
{
    m_dataMutex.lock();
    m_data.enqueue(chunk);
    m_dataMutex.unlock();
    m_pcmWaitCondition.wakeAll();
}

Chunk AudioController::nextChunk()
{
    if (!m_data.isEmpty())
        return m_data.dequeue();
    Chunk c;
    c.m_data = 0;
    c.m_dataFrames = -1;
    c.m_rate = -1;
    return c;
}

bool AudioController::hasChunk() const
{
    return !m_data.isEmpty();
}

void AudioController::clearSoundQueue()
{
    m_dataMutex.lock();
    while (!m_data.isEmpty()) {
        Chunk c = m_data.dequeue();
        free(c.m_data);
    }
    m_dataMutex.unlock();
    m_pcmMutex.lock();
    clear();
    m_pcmMutex.unlock();
}

void AudioController::trackProgress(const Chunk &chunk)
{
    int length = m_currentTrack.duration();
    if(chunk.m_dataFrames == -1) {
        m_trackPos = length;
    }
    else {
        int seconds = m_trackPos / 1000;
        m_trackPos += chunk.m_dataFrames * 1000/chunk.m_rate;
        if (m_trackPos / 1000 <= seconds)
            return;
    }
    emit trackProgress(m_trackPos / 1000);
}

void AudioController::onPlayTokenLost()
{
    emit error(PlaybackError, QString(tr("Music is being played with this account at other client")));
    stop();
}

void AudioController::run()
{
    Q_FOREVER {
        m_dataMutex.lock();
        while (!hasChunk()) {
            m_pcmWaitCondition.wait(&m_dataMutex);
        }

        Chunk c = nextChunk();
        if (c.m_dataFrames == -1) {
            m_dataMutex.unlock();
            loadNext();
            continue;
        }
        m_dataMutex.unlock();
        m_pcmMutex.lock();
        while (!isPlaying()) {
            m_playCondition.wait(&m_pcmMutex);
        }
        play(c);
        trackProgress(c);
        m_pcmMutex.unlock();
        free(c.m_data);
        QThread::usleep(5000);
    }
}
