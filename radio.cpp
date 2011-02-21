#include "radio.h"
#include "spotifysession.h"
#include "soundfeeder.h"
#include "station.h"

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
  
    initSound();
    m_soundFeeder = new SoundFeeder(this);
    connect(m_soundFeeder, SIGNAL(pcmWritten(Chunk)), this, SLOT(onPcmWritten(Chunk)));
    m_soundFeeder->start();
}

Radio::~Radio()
{
    delete m_soundFeeder;
}

void Radio::exit()
{
    m_isExiting = true;
    m_pcmWaitCondition.wakeAll();
}

void Radio::initSound()
{
    int d = 0;
    snd_pcm_uframes_t periodSize = 1024;
    snd_pcm_uframes_t bufferSize = periodSize * 4;

    snd_pcm_hw_params_t *hwParams;
    snd_pcm_open(&m_snd, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_hw_params_malloc(&hwParams);
    snd_pcm_hw_params_any(m_snd, hwParams);
    snd_pcm_hw_params_set_access(m_snd, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(m_snd, hwParams, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate(m_snd, hwParams, 44100, 0);
    snd_pcm_hw_params_set_channels(m_snd, hwParams, 2);
    snd_pcm_hw_params_set_period_size_near(m_snd, hwParams, &periodSize, &d);
    snd_pcm_hw_params_set_buffer_size_near(m_snd, hwParams, &bufferSize);
    snd_pcm_hw_params(m_snd, hwParams);
    snd_pcm_hw_params_free(hwParams);

    snd_pcm_sw_params_t *swParams;
    snd_pcm_sw_params_malloc(&swParams);
    snd_pcm_sw_params_current(m_snd, swParams);
    snd_pcm_sw_params_set_avail_min(m_snd, swParams, periodSize);
    snd_pcm_sw_params_set_start_threshold(m_snd, swParams, 0);
    snd_pcm_sw_params(m_snd, swParams);
    snd_pcm_sw_params_free(swParams);

    snd_pcm_prepare(m_snd);
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
        snd_pcm_prepare(m_snd);
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
    if(isPlaying()) {
        sp_session_player_play(SpotifySession::self()->session(), false);
        sp_session_player_unload(SpotifySession::self()->session());
        m_pcmMutex.lock();
        snd_pcm_drop(m_snd);
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
    connect(SpotifySession::self(), SIGNAL(metadataUpdated()), m_station, SLOT(onMetadataUpdated()));
    connect(m_station, SIGNAL(noArtistFound()), this, SLOT(onNoArtistFound()));
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

void Radio::onNoArtistFound()
{
    emit error(QString(tr("This item is not available from streaming")));
}

