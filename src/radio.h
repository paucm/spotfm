#ifndef RADIO_H
#define RADIO_H

#include <QObject>
#include <QMutex>
#include <QQueue>
#include <QBuffer>
#include <QWaitCondition>

#include <libspotify/api.h>

#include <ella/track.h>

#include "audio.h"
#include "chunk.h"
#include "track.h"

class PlaylistResolver;
class SpotifySession;
class SoundFeeder;

class Radio: public QObject {
  Q_OBJECT
  public:
    enum State {
        Stopped = 0,
        Playing,
        Paused
    };

    Radio();
    ~Radio();

    static Radio *self() { return s_self; }

    Audio *pcmHandle() const { return m_snd; }
    QMutex &pcmMutex() { return m_pcmMutex; }
    QMutex &dataMutex() { return m_dataMutex; }
    QWaitCondition &pcmWaitCondition() { return m_pcmWaitCondition; }
    QWaitCondition &playCondition() { return m_playCondition; }

    void newChunk(const Chunk &chunk);
    Chunk nextChunk();
    bool hasChunk() const;

    void exit();
    bool isExiting() const { return m_isExiting; }
    bool isPlaying() const { return m_state == Playing; }

    void play(const QList<ella::Track> tracks);
    void stop();
    void skipTrack();

    void pause() { setState(Paused); }
    void unpause()
    {
        setState(Playing);
        m_playCondition.wakeAll();
    }

    State state() const { return m_state; }

  signals:
    void playing(Track);
    void trackInQueue();
    void error(QString message);
    void trackProgress(int pos);

  private:
    void play();
    void setState(const State &state) { m_state = state; }

  private slots:
    void onPcmWritten(const Chunk &chunk);
    void onTrackAvailable();
    void onPlayTokenLost();

  private:
    void initSound();
    void clearSoundQueue();

  private:
    Audio *m_snd;
    QMutex m_pcmMutex;
    QMutex m_dataMutex;
    QWaitCondition m_pcmWaitCondition;
    QWaitCondition m_playCondition;
    QQueue<Chunk> m_data;
    SoundFeeder *m_soundFeeder;

    static Radio *s_self;
    PlaylistResolver *m_playlistResolver;

    Track m_currentTrack;
    int m_trackPos;

    bool m_isExiting;
    State m_state;
};

#endif
