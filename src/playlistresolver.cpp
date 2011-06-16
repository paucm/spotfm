#include <QTimer>
#include <QRegExp>

#include <ella/track.h>

#include "playlistresolver.h"
#include "spotifysession.h"
#include "spotifyquery.h"

#define QUEUE_SIZE 2


PlaylistResolver::PlaylistResolver(QObject *parent)
    : QObject(parent)
{
    m_sp_query = new SpotifyQuery(SpotifySession::self()->session());
    connect(m_sp_query, SIGNAL(queryCompleted()), this, SLOT(onQueryCompleted()));
    connect(m_sp_query, SIGNAL(queryError(QString)), this, SLOT(onQueryError(QString)));
    connect(m_sp_query, SIGNAL(queryNoResults()), this, SLOT(onQueryNoResults()));

    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(fill()));

    connect(SpotifySession::self(), SIGNAL(metadataUpdated()), this, SLOT(onMetadataUpdated()));

    m_stop = false;
}

PlaylistResolver::~PlaylistResolver()
{
    delete m_sp_query;
}

Track PlaylistResolver::takeNextTrack()
{
    if (m_queue.isEmpty())
        return Track();

    Track track = m_queue.takeFirst();
    m_timer->start();
    return track;
}

void PlaylistResolver::fill()
{
    if (m_stop) return;

    if (m_tracks.isEmpty()) {
        stop();
        return;
    }

    if (m_queue.size() < QUEUE_SIZE) {
        Track track(m_tracks.back());
        m_sp_query->execute(track);
    }
}

void PlaylistResolver::stop()
{
    m_stop = true;
    if (m_timer->isActive())
        m_timer->stop();
    m_queue.clear();
    m_pending.clear();
}

void PlaylistResolver::onMetadataUpdated()
{
    if(m_pending.isEmpty()) return;

    for (int i = 0; i < m_pending.size(); ++i) {
        Track t = m_pending.at(i);
        if (t.isAvailable()) {
            m_pending.removeAt(i);
            m_queue.append(t);
            emit trackAvailable();
        }
    }
}

void PlaylistResolver::setPlaylist(const QList<ella::Track> &tracks)
{
    if(!m_stop)
        stop();
    m_tracks = tracks;
    fill();
}

void PlaylistResolver::onQueryCompleted()
{
    if (m_stop) return;
    Track t = m_sp_query->currentTrack();
    m_pending.append(t);
    m_tracks.pop_back();
    m_timer->start();
}

void PlaylistResolver::onQueryError(const QString &msg)
{
    Track t = m_sp_query->currentTrack();
    qDebug("onQueryError: (%s - %s) : %s",
            t.artist().toLocal8Bit().constData(),
            t.title().toLocal8Bit().constData(),
            msg.toLocal8Bit().constData());
    m_tracks.pop_back();
    m_timer->start();
}

void PlaylistResolver::onQueryNoResults()
{
    Track t = m_sp_query->currentTrack();
    qDebug("onQueryNoResults: %s - %s",
            t.artist().toLocal8Bit().constData(),
            t.title().toLocal8Bit().constData());
    m_tracks.pop_back();
    m_timer->start();
}

