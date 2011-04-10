#ifndef STATION_H
#define STATION_H

#include <QObject>
#include <QString>

#include <libspotify/api.h>

#include <ella/track.h>

#include "track.h"

class SpotifyQuery;
class QTimer;

class Station : public QObject
{
    Q_OBJECT
    public:
        Station(const QString &name, QObject *parent=0)
            : QObject(parent)
            , m_name(name)
        {
        }

        ~Station()
        {
        }

        QString name() const { return m_name; }
        virtual Track takeNextTrack() = 0;

        virtual void start() = 0;
        virtual void stop() = 0;

    signals:
        void trackAvailable();
        void error(const QString &msg);

    private:
        QString m_name;
};

class QueryStation : public Station
{
    Q_OBJECT
    public:
        QueryStation(const QString &name, QObject *parent = 0);
        ~QueryStation();

        virtual Track takeNextTrack();
        virtual void stop();
        virtual void start() { m_stop = false; fill(); }

    protected:
        virtual void search() = 0;
        void setTracks(const QList<ella::Track> &tracks);
        
    protected slots:
        void onQueryCompleted(const Track &t);
        void onQueryError(const QString &q, const QString &msg);
        void onQueryNoResults(const QString &q);

        void fill();
        void onMetadataUpdated();

    private:
        bool m_stop;
        SpotifyQuery *m_sp_query;

        QList<ella::Track> m_tracks;
        QList<Track> m_queue;
        QList<Track> m_pending;
        QTimer *m_timer;
};

#endif
