#ifndef AUDIO_H
#define AUDIO_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

#include "track.h"
#include "chunk.h"
#include "spotifysession.h"
#include "util.h"


class AudioController : public QThread
{
    Q_OBJECT

    public:
        AudioController(QObject *parent=0);
        virtual ~AudioController();

        bool isPlaying() const;
        void play();
        void stop();
        void loadNext();
        void pause();
        void unpause();

        void enqueue(const Track &track);
        void clearQueue();
        Track currentTrack() const { return m_currentTrack; }

	    virtual void init() = 0;
        virtual void play(Chunk &chunk) = 0;
        virtual void prepare() = 0;
        virtual float volume() = 0;

        RadioState state() const { return m_state; }

    signals:
        void trackStarted();
        void trackEnded(int seconds);
        void trackProgress(int seconds);
        void error(int code, const QString &message);

    public slots:
        virtual void setVolume(int volume) = 0;
        virtual void clear() = 0;

    private slots:
        void onPlayTokenLost();

    private:
        void run();
        void trackProgress(const Chunk &chunk);

        void newChunk(const Chunk &chunk);
        Chunk nextChunk();
        bool hasChunk() const;
        void clearSoundQueue();

        QQueue<Track> m_playlist;
        Track m_currentTrack;
        int m_trackPos;

        QMutex m_pcmMutex;
        QMutex m_dataMutex;
        QWaitCondition m_pcmWaitCondition;
        QWaitCondition m_playCondition;
        QQueue<Chunk> m_data;

        RadioState m_state;

        friend class SpotifySession;
};


#endif
