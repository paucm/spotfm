#ifndef RADIO_H
#define RADIO_H

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
class Station;

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
    bool isPlaying() const { return m_state == Playing; }

    void playStation(Station *station);
    Station *station() const { return m_station; }

    void pause() { setState(Paused); }
    void unpause()
    {
        setState(Playing);
        m_playCondition.wakeAll();
    }
    void stopStation();
    void skipTrack();

    State state() const { return m_state; }

  signals:
    void playing(Track);
    void trackInQueue();
    void error(QString message);

  private:
    void play();
    void setState(const State &state) { m_state = state; }

  private slots:
    void onEndOfTrack();
    void onPcmWritten(const Chunk &chunk);
    void onTrackAvailable();
    void onNoArtistFound();

  private:
    void initSound();
    void clearSoundQueue();

  private:
    snd_pcm_t *m_snd;
    QMutex m_pcmMutex;
    QMutex m_dataMutex;
    QWaitCondition m_pcmWaitCondition;
    QWaitCondition m_playCondition;
    QQueue<Chunk> m_data;
    SoundFeeder *m_soundFeeder;

    static Radio *s_self;
    Station *m_station;

    Track m_currentTrack;
    int m_trackPos;

    bool m_isExiting;
    State m_state;
};

#endif
