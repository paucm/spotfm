#include "player.h"
#include "spotifysession.h"
#include "soundfeeder.h"
#include "playlist.h"

#include <QCoreApplication>
#include <QFile>

#include <iostream>

Player *Player::s_self = 0;

Player::Player(const QString &user, const QString &password, const QString &query)
 : m_isPlaying(false)
 , m_currentTrack()
 , m_trackPos(0)
 , m_isExiting(false)
{
  
  qRegisterMetaType<Chunk>();
  s_self = this;
  
  m_sp_session = new SpotifySession();
  m_playlist = new Playlist(query, m_sp_session->session());
  
  connect(m_sp_session, SIGNAL(loggedError(QString)), this, SLOT(onLoggedError(QString)));
  connect(m_sp_session, SIGNAL(loggedIn()), this, SLOT(onLoggedIn()));
  connect(m_sp_session, SIGNAL(loggedOut()), this, SLOT(onLoggedOut()));
  connect(m_sp_session, SIGNAL(metadataUpdated()), m_playlist, SLOT(onMetadataUpdated()));
//   connect(m_sp_session, SIGNAL(endOfTrack()), this, SLOT(onEndOfTrack()));
  connect(m_playlist, SIGNAL(trackAvailable()), this, SLOT(onTrackAvailable()));
  

  
  m_sp_session->login(user.toLocal8Bit().constData(), password.toLocal8Bit().constData());
  
  initSound();
  m_soundFeeder = new SoundFeeder(this);
  connect(m_soundFeeder, SIGNAL(pcmWritten(Chunk)), this, SLOT(onPcmWritten(Chunk)));
  m_soundFeeder->start();
}

Player::~Player()
{
  delete m_sp_session;
}

void Player::exit()
{
  m_isExiting = true;
  m_pcmWaitCondition.wakeAll();
  m_sp_session->logout();
}

void Player::initSound()
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

void Player::onLoggedError(const QString &msg)
{
  std::cerr << "loggin error: " << msg.toLocal8Bit().constData() << std::endl;
  QCoreApplication::exit();
}
    
void Player::onLoggedIn()
{
  std::cout << "logged in Spotify" << std::endl;
  m_playlist->fill();
}

void Player::onLoggedOut()
{
  std::cout << "logged out" << std::endl;
  QCoreApplication::exit();
}

void Player::onTrackAvailable()
{
  if(m_isPlaying == false) {
    play();
  }
}

void Player::play()
{
  m_currentTrack = m_playlist->takeNextTrack();
  std::cout << "Playing " << m_currentTrack.artist().toLocal8Bit().constData() << " - " << m_currentTrack.title().toLocal8Bit().constData() << std::endl;
  clearSoundQueue();
  m_isPlaying = true;
  m_pcmMutex.lock();
  snd_pcm_prepare(m_snd);
  m_pcmMutex.unlock();
  sp_session_player_load(m_sp_session->session(), m_currentTrack.spotifyTrack());
  sp_session_player_play(m_sp_session->session(), true);
  m_playCondition.wakeAll();
}


void Player::onEndOfTrack()
{
//   std::cout << "Finished" << std::endl;
//   sp_track_release(m_track);
//   m_sp_session->logout();
}


void Player::newChunk(const Chunk &chunk) 
{
  m_data.enqueue(chunk);
}

Chunk Player::nextChunk()
{
  return m_data.dequeue();
}

bool Player::hasChunk() const
{
  return !m_data.isEmpty();
}

void Player::onPcmWritten(const Chunk &chunk)
{
  int length = m_currentTrack.duration();
  m_trackPos += chunk.m_dataFrames * 1000/chunk.m_rate;
  if(m_trackPos >= length * 0.98) {
    play();
  }
//   else {
//      QString total = QString("%1:%2").arg((length / 1000) / 60, 2, 10, QLatin1Char('0'))
//                                      .arg((length / 1000) % 60, 2, 10, QLatin1Char('0'));
//      QString progress = QString("%1:%2").arg((quint64) (m_trackPos / 60, 2, 10, QLatin1Char('0')))
//                                         .arg((quint64) (m_trackPos % 60, 2, 10, QLatin1Char('0')));
//      std::cout << progress.toLocal8Bit().constData() << " of " << total.toLocal8Bit().constData() << '\r' << std::flush;
//   }
}

void Player::clearSoundQueue()
{
    m_dataMutex.lock();
    if(m_isPlaying) {
      sp_session_player_play(m_sp_session->session(), false);
      sp_session_player_unload(m_sp_session->session());
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


