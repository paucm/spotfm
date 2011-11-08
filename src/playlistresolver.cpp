#include <QTimer>
#include <QRegExp>
#include <QDebug>

#include <ella/track.h>

#include "playlistresolver.h"
#include "spotifysession.h"
#include "spotifyquery.h"

#define QUEUE_SIZE 1
#define ARTIST_HISTORY_LIMIT 5


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

    m_isRunning = false;
}

PlaylistResolver::~PlaylistResolver()
{
    delete m_sp_query;
}

Track PlaylistResolver::takeNextTrack()
{
    if (m_queue.isEmpty())
        return Track();
    return m_queue.takeFirst();
}

void PlaylistResolver::fill()
{
    if (!m_isRunning) return;

    Q_FOREVER {
        if (m_tracks.isEmpty()) {
            m_isRunning = false;
            emit done();
            break;
        }
        Track track(m_tracks.back());
        m_tracks.pop_back();

        if (m_artistHistory.contains(track.artist())) {
            qDebug() << "Ignored artist " << track.artist();
            continue;
        }
        m_sp_query->execute(track);
        break;
    }
}

void PlaylistResolver::start()
{
    if (m_isRunning) return;
    m_isRunning = true;
    fill();
}


void PlaylistResolver::stop()
{
    if (!m_isRunning) return;
    m_isRunning = false;
    if (m_timer->isActive())
        m_timer->stop();
    m_artistHistory.clear();
    m_queue.clear();
    m_pending.clear();
}

void PlaylistResolver::onMetadataUpdated()
{
    if (m_pending.isEmpty()) return;

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
    m_tracks = tracks;
}

void PlaylistResolver::onQueryCompleted()
{
    if (!m_isRunning) return;
    Track t = m_sp_query->currentTrack();
    m_artistHistory.append(t.artist());
    if (m_artistHistory.size() >= ARTIST_HISTORY_LIMIT)
        m_artistHistory.pop_front();
    m_pending.append(t);
    m_timer->start();
}

void PlaylistResolver::onQueryError(const QString &msg)
{
    Q_UNUSED(msg);
    if (!m_isRunning) return;
    Track t = m_sp_query->currentTrack();
    m_timer->start();
}

void PlaylistResolver::onQueryNoResults()
{
    if (!m_isRunning) return;
    Track t = m_sp_query->currentTrack();
    m_timer->start();
}
