#include <QTimer>

#include "station.h"
#include "spotifyquery.h"
#include "spotifysession.h"
#include "selector.h"

#define QUEUE_SIZE 2
#define ARTIST_HISTORY_LIMIT 5

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

    m_artistSelector = 0;
    m_stop = false;
}

QueryStation::~QueryStation()
{
    if (m_artistSelector)
        delete m_artistSelector;
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

    if (!m_artistSelector) {
        search();
        return;
    }

    if (m_artistSelector->size() <= ARTIST_HISTORY_LIMIT) {
        return;
    }

    if (m_queue.size() < QUEUE_SIZE) {
        QString artist;
        do {
            artist = m_artistSelector->getItem();
        } while(m_artistHistory.contains(artist));

        m_sp_query->execute(artist);
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

void QueryStation::createArtistSelector(QMap<int, QString> artists)
{
    if (m_artistSelector) delete m_artistSelector;
    m_artistSelector =  new Selector(artists);
    fill();
}

void QueryStation::onQueryCompleted(const Track &t)
{
    if (m_stop) return;
    m_artistHistory << t.artist();
    if(m_artistHistory.size() >= ARTIST_HISTORY_LIMIT) {
        m_artistHistory.removeFirst();
    }
    m_pending.append(t);
    m_timer->start();
}

void QueryStation::onQueryError(const QString &query, const QString &msg)
{
    qDebug("onQueryError: %s : %s", query.toLocal8Bit().constData(), msg.toLocal8Bit().constData());
    m_artistSelector->removeValue(query);
    m_timer->start();
}

void QueryStation::onQueryNoResults(const QString &query)
{
    qDebug("onQueryNoResults: %s", query.toLocal8Bit().constData());
    m_artistSelector->removeValue(query);
    m_timer->start();
}
