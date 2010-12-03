#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QMutex>
#include <QQueue>
#include <QBuffer>
#include <QWaitCondition>

#include <libspotify/api.h>
#include <alsa/asoundlib.h>

#include "chunk.h"
#include "track.h"

class SpotifySession;
class SpotifyQuery;
class SoundFeeder;
class Playlist;

class Player: public QObject {
  Q_OBJECT
  public:
    Player(const QString &user, const QString &password, const QString &query);
    ~Player();
    
    static Player *self() { return s_self; }

    snd_pcm_t * pcmHandle() const { return m_snd; }
    QMutex &pcmMutex() { return m_pcmMutex; }
    QMutex &dataMutex() { return m_dataMutex; }
    QWaitCondition &pcmWaitCondition() { return m_pcmWaitCondition; }
    QWaitCondition &playCondition() { return m_playCondition; }
    
    void newChunk(const Chunk &chunk);
    Chunk nextChunk();
    bool hasChunk() const;
    
    void exit();
    bool isExiting() const { return m_isExiting; }
     
  private:
    void play();
    
  private slots:
    void onLoggedError(const QString &msg);
    void onLoggedIn();
    void onLoggedOut();
    void onEndOfTrack();
    void onPcmWritten(const Chunk &chunk);
    void onTrackAvailable();
    
  private:
    void initSound();
    void clearSoundQueue();
    
  private:
    snd_pcm_t            *m_snd;
    QMutex                m_pcmMutex;
    QMutex                m_dataMutex;
    QWaitCondition        m_pcmWaitCondition;
    QWaitCondition        m_playCondition;
    QQueue<Chunk>         m_data;
    SoundFeeder          *m_soundFeeder;
    
    SpotifySession       *m_sp_session;
    static Player        *s_self;    
    bool                 m_isPlaying;
    Playlist             *m_playlist;
    
    Track                m_currentTrack;
    int                  m_trackPos;
    
    bool                  m_isExiting;
};




#endif
