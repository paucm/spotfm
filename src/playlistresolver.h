#ifndef PLAYLISTRESOLVER_H
#define PLAYLISTRESOLVER_H

#include <QObject>
#include <QString>

#include <ella/track.h>

#include "track.h"

class SpotifyQuery;
class QTimer;


class PlaylistResolver: public QObject
{
    Q_OBJECT
    public:
        PlaylistResolver(QObject *parent = 0);
        ~PlaylistResolver();

        virtual Track takeNextTrack();
        virtual void stop();
        virtual void start();
        bool isRunning() const { return m_isRunning; }

        void setPlaylist(const QList<ella::Track> &tracks);

    signals:
        void trackAvailable();
        void done();

    protected slots:
        void onQueryCompleted();
        void onQueryError(const QString &msg);
        void onQueryNoResults();

        void fill();
        void onMetadataUpdated();

    private:
        bool m_isRunning;
        SpotifyQuery *m_sp_query;

        QList<ella::Track> m_tracks;
        QList<Track> m_queue;
        QList<Track> m_pending;
        QTimer *m_timer;
        QStringList m_artistHistory;
};

#endif
