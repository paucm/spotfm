#include <cstdlib>

#include "radio.h"
#include "playlistresolver.h"
#include "spotifysession.h"
#include "soundfeeder.h"
#include "openal-audio.h"

#include <QCoreApplication>
#include <QFile>


Radio::Radio()
 : m_currentTrack()
 , m_trackPos(0)
 , m_isExiting(false)
 , m_state(Stopped)
{
    qRegisterMetaType<Chunk>();

    m_playlistResolver = new PlaylistResolver(this);

    connect(m_playlistResolver, SIGNAL(trackAvailable()), this, SLOT(onTrackAvailable()));
    //connect(m_playlistResolver, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
    connect(SpotifySession::self(), SIGNAL(playTokenLost()), this, SLOT(onPlayTokenLost()));

    initSound();
    m_soundFeeder = new SoundFeeder(this);
    connect(m_soundFeeder, SIGNAL(pcmWritten(Chunk)), this, SLOT(onPcmWritten(Chunk)));
    m_soundFeeder->start();
}

Radio::~Radio()
{
    exit();
    delete m_snd;
    delete m_soundFeeder;
}

void Radio::exit()
{
    stop();
    m_isExiting = true;
    m_pcmWaitCondition.wakeAll();
    m_playCondition.wakeAll();
    m_soundFeeder->wait();
}

void Radio::initSound()
{
	m_snd = new OpenalAudio();
	m_snd->init();
}

void Radio::onTrackAvailable()
{
    if (isPlaying() == false) {
        play();
    }
    else {
        emit trackInQueue();
    }
}

void Radio::play()
{
    m_currentTrack = m_playlistResolver->takeNextTrack();
    if (m_currentTrack.isValid()) {
        clearSoundQueue();
        m_pcmMutex.lock();
		m_snd->prepare();
        m_pcmMutex.unlock();
        sp_session_player_load(SpotifySession::self()->session(), m_currentTrack.spotifyTrack());
        sp_session_player_play(SpotifySession::self()->session(), true);
        m_playCondition.wakeAll();
        setState(Playing);
        qDebug("Playing %s - %s",
                m_currentTrack.artist().toLocal8Bit().constData(),
                m_currentTrack.title().toLocal8Bit().constData());
        emit playing(m_currentTrack);
    }
    else {
        stop();
        emit error(QString(tr("Playlist finished")));
    }
}

void Radio::newChunk(const Chunk &chunk)
{
    m_data.enqueue(chunk);
}

Chunk Radio::nextChunk()
{
    if (!m_data.isEmpty())
        return m_data.dequeue();
    Chunk c;
    c.m_data = 0;
    c.m_dataFrames = -1;
    c.m_rate = -1;
    return c;
}

bool Radio::hasChunk() const
{
    return !m_data.isEmpty();
}

void Radio::onPcmWritten(const Chunk &chunk)
{
    int length = m_currentTrack.duration();
    if(chunk.m_dataFrames == -1) {
        m_trackPos = length; 
    }
    else {
        m_trackPos += chunk.m_dataFrames * 1000/chunk.m_rate;
        emit trackProgress(m_trackPos);
    }

    if(m_trackPos >= length) {
        play();
    }
}

void Radio::clearSoundQueue()
{
    m_dataMutex.lock();
    if(state() != Stopped) {
        m_pcmMutex.lock();
        m_snd->clear();
        m_pcmMutex.unlock();
        while (!m_data.isEmpty()) {
            Chunk c = m_data.dequeue();
            free(c.m_data);
        }
    }
    m_trackPos = 0;
    m_dataMutex.unlock();
}

void Radio::play(const QList<ella::Track> tracks)
{
    m_playlistResolver->setPlaylist(tracks);
    m_playlistResolver->start();
}

void Radio::stop()
{
    if (state() != Stopped) {
        m_playlistResolver->stop();
        sp_session_player_unload(SpotifySession::self()->session());
        clearSoundQueue();
        setState(Stopped);
    }
}

void Radio::skipTrack()
{
    sp_session_player_play(SpotifySession::self()->session(), false);
    sp_session_player_unload(SpotifySession::self()->session());
    play();
}

void Radio::onPlayTokenLost()
{
    stop();
    emit error(QString(tr("Music is being played with this account at other client")));
}

void Radio::setVolume(int volume)
{
    m_snd->setVolume(volume);
}
