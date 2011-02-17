#ifndef SPOTIFYQUERY_H
#define SPOTIFYQUERY_H

#include <QObject>
#include <QStringList>
#include <QList>

#include "track.h"

#include <libspotify/api.h>

class SpotifyQuery : public QObject
{
    Q_OBJECT

    public:
        SpotifyQuery(sp_session *session);
        ~SpotifyQuery();

        void execute(const QString &query);

    signals:
        void queryCompleted(const Track &track);
        void queryError(const QString &query, const QString &msg);
        void queryNoResults(const QString &query);

    private:
        //Spotify callback
        static void searchComplete(sp_search *search, void *userdata);

        typedef QList<sp_track *> TrackList;
        Track bestTrack(TrackList candidates);

        sp_session *m_session;
        sp_search *m_search;
        QString m_query;
        QStringList m_tracks;
};

#endif
