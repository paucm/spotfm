#include <stdexcept>
#include <ctime>

#include <QNetworkReply>
#include <QTimer>

#include <lastfm/Artist>

#include "station.h"
#include "spotifysession.h"
#include "spotifyquery.h"

#define QUEUE_SIZE 3
#define ARTIST_HISTORY_LIMIT 5

Station::Station(const QString &name, QObject *parent)
 : QObject(parent)
 , m_name(name)
 , m_stop(false)
{
    m_sp_query = new SpotifyQuery(SpotifySession::self()->session());
  
    connect(m_sp_query, SIGNAL(queryCompleted(Track)), this, SLOT(onQueryCompleted(Track)));
    connect(m_sp_query, SIGNAL(queryError(QString, QString)), this, SLOT(onQueryError(QString, QString)));
    connect(m_sp_query, SIGNAL(queryNoResults(QString)), this, SLOT(onQueryNoResults(QString)));
  
    time_t now;
    now = time(NULL);
    qsrand(now);
}

Station::~Station()
{
    delete m_sp_query;
}

void Station::fill()
{
    if (m_stop) return;

    if (m_artists.isEmpty()) {
        search();
        return;
    }
  
    if (m_queue.size() < QUEUE_SIZE) {
        QString artist = m_artists.at(qrand() % m_artists.size());
        while(m_artistHistory.contains(artist)) {
          artist = m_artists.at(qrand() % m_artists.size());
        }
    
    m_artistHistory << artist;
    if(m_artistHistory.size() >= ARTIST_HISTORY_LIMIT) {
      m_artistHistory.removeFirst();
    }
    m_sp_query->execute(artist);
  }
}

Track Station::takeNextTrack()
{
    if (m_queue.isEmpty()) 
        return Track();
 
    Track track = m_queue.takeFirst();
    fillAgain();
    return track;
}

void Station::search()
{
    QNetworkReply *reply = lastfm::Artist(m_name).getSimilar();
    connect( reply, SIGNAL(finished()), SLOT(onGotSearch()));
}

void Station::onGotSearch()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() != QNetworkReply::NoError) {
        emit noArtistFound();
        m_stop = true;
        return;
    }

    m_artists = lastfm::Artist::getSimilar( reply ).values();
    fill();
}

void Station::onMetadataUpdated()
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

void Station::fillAgain()
{
    if (m_stop) return;
    QTimer::singleShot(500, this, SLOT(fill()));
}
  
void Station::onQueryCompleted(const Track &t)
{
    m_pending.append(t);
    fillAgain();
}
    
void Station::onQueryError(const QString &query, const QString &msg)
{
    qDebug("onQueryError: %s : %s", query.toLocal8Bit().constData(), msg.toLocal8Bit().constData());
    fillAgain();
}
    
void Station::onQueryNoResults(const QString &query)
{
    qDebug("onQueryNoResults: %s", query.toLocal8Bit().constData());
    fillAgain();
}
