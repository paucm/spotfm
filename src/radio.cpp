#include <cstdlib>

#include "radio.h"
#include "spotifysession.h"
#include "soundfeeder.h"
#include "station.h"
#ifdef Q_OS_WIN32
#include "openal-audio.h"
#else
#include "alsa-audio.h"
#endif

#include <QCoreApplication>
#include <QFile>

Radio *Radio::s_self = 0;

Radio::Radio()
 : m_currentTrack()
 , m_trackPos(0)
 , m_isExiting(false)
 , m_state(Stopped)
{
  
    qRegisterMetaType<Chunk>();
    s_self = this;

    connect(SpotifySession::self(), SIGNAL(playTokenLost()), this, SLOT(onPlayTokenLost()));
  
    initSound();
    m_soundFeeder = new SoundFeeder(this);
    connect(m_soundFeeder, SIGNAL(pcmWritten(Chunk)), this, SLOT(onPcmWritten(Chunk)));
    m_soundFeeder->start();
}

Radio::~Radio()
{
	delete m_snd;
    delete m_soundFeeder;
}

void Radio::exit()
{
    m_isExiting = true;
    m_pcmWaitCondition.wakeAll();
}

void Radio::initSound()
{
#ifdef Q_OS_WIN32
	m_snd = new OpenalAudio();
#else
	m_snd = new AlsaAudio();
#endif
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
    m_currentTrack = m_station->takeNextTrack();
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
        stopStation();
        emit error(QString(tr("Playlist finished")));
    }
}

void Radio::newChunk(const Chunk &chunk) 
{
    m_data.enqueue(chunk);
}

Chunk Radio::nextChunk()
{
    return m_data.dequeue();
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
        sp_session_player_play(SpotifySession::self()->session(), false);
        sp_session_player_unload(SpotifySession::self()->session());
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

void Radio::playStation(Station *station)
{
    m_station = station;
    connect(m_station, SIGNAL(trackAvailable()), this, SLOT(onTrackAvailable()));
    connect(m_station, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
    m_station->start();
}

void Radio::stopStation()
{
    m_station->stop();
    sp_session_player_play(SpotifySession::self()->session(), false);
    sp_session_player_unload(SpotifySession::self()->session());
    clearSoundQueue();
    setState(Stopped);
}

void Radio::skipTrack()
{
    sp_session_player_play(SpotifySession::self()->session(), false);
    sp_session_player_unload(SpotifySession::self()->session());
    play();
}

void Radio::onPlayTokenLost()
{
    stopStation();
    emit error(QString(tr("Music is being played with this account at other client")));
}