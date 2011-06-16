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

        void execute(const Track &track);
        Track currentTrack() const { return m_track; }

    signals:
        void queryCompleted();
        void queryError(const QString &msg);
        void queryNoResults();

    private:
        //Spotify callback
        static void SP_CALLCONV searchComplete(sp_search *search, void *userdata);
        sp_session *m_session;
        sp_search *m_search;
        Track m_track;
};

#endif
