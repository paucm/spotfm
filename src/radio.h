#ifndef RADIO_H
#define RADIO_H

#include <QObject>

#include "audiocontroller.h"
#include "chunk.h"
#include "track.h"
#include "util.h"

class PlaylistResolver;
class SpotifySession;


class Radio: public QObject {
  Q_OBJECT

  public:
    typedef QList<ella::Track> Playlist;

    Radio(QObject *parent=0);
    ~Radio();

    QString stationName() const
    {
        return QString(tr("similar to %1")).arg(m_name);
    }
    void playStation(const QString &name);

    RadioState state() const { return m_state; }

    AudioController *audioController() { return m_audioController; }

  public slots:
    void stop();
    void skipTrack();
    void setVolume(int volume);
    void pause();
    void unpause();

  signals:
    void trackStarted(Track track);
    void trackEnded(int at);
    void skipsLeft(int skips);
    void trackProgress(int pos);

    void error(int code, const QString &message);

  private slots:
    void onGotPlaylist();
    void onTrackAvailable();
    void onTrackStarted();
    void onTrackEnded(int at);
    void onAudioControllerError(int code, const QString &message);

  private:
    void setState(const RadioState &state) { m_state = state; }

    AudioController *m_audioController;
    PlaylistResolver *m_playlistResolver;

    int m_trackPos;

    int m_skipLeft;

    QString m_name;
    RadioState m_state;
};

#endif

