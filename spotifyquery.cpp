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
    QString q = QString("artist:%1").arg(query);
    m_search = sp_search_create(m_session, q.toUtf8().data(), 0, 5, 0, 0, 0, 0, &SpotifyQuery::searchComplete, this);
    if (!m_search) {
        qDebug("Failed to start search");
        return;
    }
}

Track SpotifyQuery::bestTrack(SpotifyQuery::TrackList candidates)
{
    unsigned int length = candidates.size();
    if (length == 0) return Track();

    int index = qrand() % length;
    for (unsigned int i=0; i<length; i++) {
        Track track(candidates.at(index));
        QString uri = track.uri();
        if (!m_tracks.contains(uri)) {
            m_tracks << uri;
            qDebug("Get track: %s", uri.toLocal8Bit().constData());
            return track;
        }
        index = (index + 1) % length;
    }
    return Track();
}

void SpotifyQuery::searchComplete(sp_search *search, void *userdata)
{
    TrackList candidates;

    SpotifyQuery *sq = static_cast<SpotifyQuery *>(userdata);
    if (search && SP_ERROR_OK == sp_search_error(search)) {
        int num_tracks = sp_search_num_tracks(search);
        if(!num_tracks) {
            emit sq->queryNoResults(sq->m_query);
        }
        else {
            for(int i=0; i < num_tracks; i++) {
                sp_track *track = sp_search_track(search, i);
                sp_artist *artist = sp_track_artist(track, 0);
                if(artist and QString::fromUtf8(sp_artist_name(artist)).toLower() == sq->m_query.toLower()) {
                    candidates.append(track);
                }
            }
            Track track = sq->bestTrack(candidates);
            if (track.isValid()) {
                emit sq->queryCompleted(track);
            }
            else {
                emit sq->queryNoResults(sq->m_query);
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

