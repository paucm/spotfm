#include "spotifyquery.h"

SpotifyQuery::SpotifyQuery(sp_session *session)
    : m_session(session)
    , m_search(0)
{
}

SpotifyQuery::~SpotifyQuery()
{
}

void SpotifyQuery::execute(const Track &track)
{
    m_track = track;
    QString query = QString("artist:%1 track:%2").arg(track.artist(), track.title());
    query.replace(QRegExp("\\[[^\\]]*\\]"), "");
    query.replace(QRegExp("\\([^\\)]*\\)"), "");
    qDebug("Spotify query: [%s] ", query.toLocal8Bit().constData());
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
            emit sq->queryNoResults();
        }
        else {
            for(int i=0; i < num_tracks; i++) {
                sq->m_track.setSpotifyTrack(sp_search_track(search, i));
                if (sq->m_track.isValid()) {
                    emit sq->queryCompleted();
                }
                else {
                    emit sq->queryNoResults();
                }
            }
        }
    }
    else {
        QString msg = sp_error_message(sp_search_error(search));
        emit sq->queryError(msg);
    }
    sp_search_release(search);
    sq->m_search = 0;
}

