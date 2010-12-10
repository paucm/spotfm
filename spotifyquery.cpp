#include "spotifyquery.h"
#include <QList>
#include <ctime>

SpotifyQuery::SpotifyQuery(sp_session *session)
    : m_session(session)
    , m_search(0)
{
    time_t now;
    now = time(NULL);
    qsrand(now);
}
 
SpotifyQuery::~SpotifyQuery()
{
}

void SpotifyQuery::execute(const QString &query)
{
    qDebug("Spotify Query execute %s", query.toLocal8Bit().constData());
    m_query = query;
    QString q = QString("artist:%1").arg(query);
    m_search = sp_search_create(m_session, q.toUtf8().data(), 0, 5, 0, 0, 0, 0, &SpotifyQuery::searchComplete, this);
    if (!m_search) {
        qDebug("Failed to start search");
        return;
    }
}

void SpotifyQuery::searchComplete(sp_search *search, void *userdata)
{
    QList<sp_track *> candidates;
    
    SpotifyQuery *sr = static_cast<SpotifyQuery *>(userdata);
    if (search && SP_ERROR_OK == sp_search_error(search)) {
        int num_tracks = sp_search_num_tracks(search);
        if(!num_tracks) {
            qDebug("noResults");
            emit sr->queryNoResults(sr->m_query);
        }
        else {
            for(int i=0; i < num_tracks; i++) {
                sp_track *track = sp_search_track(search, i);
                sp_artist *artist = sp_track_artist(track, 0);
                if(artist and QString::fromUtf8(sp_artist_name(artist)).toLower() == sr->m_query.toLower()) {
                    candidates.append(track);
                } 
            }
            if(candidates.size() > 0) {
                Track track(candidates.at(qrand() % candidates.size()));
                qDebug("queryCompleted");
                emit sr->queryCompleted(track);
            }
            else {
                qDebug("noResults");
                emit sr->queryNoResults(sr->m_query);
            }
        }
    }
    else {
        QString msg = sp_error_message(sp_search_error(search));
        qDebug("queryError");
        emit sr->queryError(sr->m_query, msg);
    }
    sp_search_release(search);
    sr->m_search = 0;
}

