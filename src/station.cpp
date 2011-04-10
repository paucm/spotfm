#include <QTimer>
#include <QRegExp>

#include <ella/track.h>

#include "station.h"
#include "spotifyquery.h"
#include "spotifysession.h"

#define QUEUE_SIZE 2

QueryStation::QueryStation(const QString &name, QObject *parent)
    : Station(name, parent)
{
    m_sp_query = new SpotifyQuery(SpotifySession::self()->session());
    connect(m_sp_query, SIGNAL(queryCompleted(Track)), this, SLOT(onQueryCompleted(Track)));
    connect(m_sp_query, SIGNAL(queryError(QString, QString)), this, SLOT(onQueryError(QString, QString)));
    connect(m_sp_query, SIGNAL(queryNoResults(QString)), this, SLOT(onQueryNoResults(QString)));

    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(fill()));

    connect(SpotifySession::self(), SIGNAL(metadataUpdated()), this, SLOT(onMetadataUpdated()));

    m_stop = false;
}

QueryStation::~QueryStation()
{
    delete m_sp_query;
}

Track QueryStation::takeNextTrack()
{
    if (m_queue.isEmpty())
        return Track();

    Track track = m_queue.takeFirst();
    m_timer->start();
    return track;
}

void QueryStation::fill()
{
    if (m_stop) return;

    if (m_tracks.isEmpty()) {
        search();
        return;
    }
    
    if (m_queue.size() < QUEUE_SIZE) {
        ella::Track track = m_tracks.back();
        QString query = QString("artist:%1 track:%2").arg(track.artistName(), track.title());
        query.replace(QRegExp("\\[[^\\]]*\\]"), "");
        query.replace(QRegExp("\\([^\\)]*\\)"), "");
        m_sp_query->execute(query);
    }
}

void QueryStation::stop()
{
    m_stop = true;
    if (m_timer->isActive())
        m_timer->stop();
}

void QueryStation::onMetadataUpdated()
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

void QueryStation::setTracks(const QList<ella::Track> &tracks)
{
    m_tracks = tracks;
    fill();
}

void QueryStation::onQueryCompleted(const Track &t)
{
    if (m_stop) return;
    
    m_pending.append(t);
    m_tracks.pop_back();
    m_timer->start();
}

void QueryStation::onQueryError(const QString &query, const QString &msg)
{
    qDebug("onQueryError: %s : %s", query.toLocal8Bit().constData(), msg.toLocal8Bit().constData());
    m_tracks.pop_back();
    m_timer->start();
}

void QueryStation::onQueryNoResults(const QString &query)
{
    qDebug("onQueryNoResults: %s", query.toLocal8Bit().constData());
    m_tracks.pop_back();
    m_timer->start();
}
