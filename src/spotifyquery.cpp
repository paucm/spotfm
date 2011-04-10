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
    m_query = query;
    m_search = sp_search_create(m_session, query.toUtf8().data(), 0, 1, 0, 0, 0, 0, &SpotifyQuery::searchComplete, this);
    if (!m_search) {
        qDebug("Failed to start search");
        return;
    }
}

void SP_CALLCONV SpotifyQuery::searchComplete(sp_search *search, void *userdata)
{
    SpotifyQuery *sq = static_cast<SpotifyQuery *>(userdata);
    if (search && SP_ERROR_OK == sp_search_error(search)) {
        int num_tracks = sp_search_num_tracks(search);
        if(!num_tracks) {
            emit sq->queryNoResults(sq->m_query);
        }
        else {
            for(int i=0; i < num_tracks; i++) {
                Track track(sp_search_track(search, i));
                if (track.isValid()) {
                    emit sq->queryCompleted(track);
                }
                else {
                    emit sq->queryNoResults(sq->m_query);
                }
            }
        }
    }
    else {
        QString msg = sp_error_message(sp_search_error(search));
        emit sq->queryError(sq->m_query, msg);
    }
    sp_search_release(search);
    sq->m_search = 0;
}

