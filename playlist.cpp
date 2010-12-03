#include "playlist.h"
#include "spotifyquery.h"

#include <QNetworkReply>
#include <QTimer>

#include <lastfm/Artist>
#include <iostream>
#include <ctime>

#define ARTIST_HISTORY_LIMIT 4

Playlist::Playlist(const QString &name, sp_session *session)
 : m_name(name)
{
  m_sp_query = new SpotifyQuery(session);
  
  connect(m_sp_query, SIGNAL(queryCompleted(Track)), this, SLOT(onQueryCompleted(Track)));
  connect(m_sp_query, SIGNAL(queryError(QString, QString)), this, SLOT(onQueryError(QString, QString)));
  connect(m_sp_query, SIGNAL(queryNoResults(QString)), this, SLOT(onQueryNoResults(QString)));
  
  time_t now;
  now = time(NULL);
  qsrand(now);
}

Playlist::~Playlist()
{
  delete m_sp_query;
}

void Playlist::fill()
{
  if (m_artists.isEmpty()) {
    search();
    return;
  }
  if (m_queue.size() < 5) {
    QString artist = m_artists.at(qrand() % m_artists.size());
    while(m_artistHistory.contains(artist)) {
      artist = m_artists.at(qrand() % m_artists.size());
    }
    std::cout << "fill()" << artist.toLatin1().constData() << std::endl;
    m_artistHistory << artist;
    if(m_artistHistory.size() >= ARTIST_HISTORY_LIMIT) {
      m_artistHistory.removeFirst();
    }
    m_sp_query->execute(artist);
  }
}

Track Playlist::takeNextTrack()
{
  if (m_queue.isEmpty()) 
    return 0;
 
  Track track = m_queue.takeFirst();
  QTimer::singleShot(500, this, SLOT(fill()));
  return track;
}

void Playlist::search()
{
  QNetworkReply *reply = lastfm::Artist(m_name).getSimilar();
  connect( reply, SIGNAL(finished()), SLOT(onGotSearch()));
}

void Playlist::onGotSearch()
{
  QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
  m_artists = lastfm::Artist::getSimilar( reply ).values();
  fill();
}

void Playlist::onMetadataUpdated()
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
  
void Playlist::onQueryCompleted(const Track &t)
{
  m_pending.append(t);
  QTimer::singleShot(100, this, SLOT(fill()));
}
    
void Playlist::onQueryError(const QString &query, const QString &msg)
{
  std::cerr << "Opps! Failed to search " << query.toLatin1().constData() << " in Spotify: " <<  msg.toLocal8Bit().constData() << std::endl;
  QTimer::singleShot(100, this, SLOT(fill()));
}
    
void Playlist::onQueryNoResults(const QString &query)
{
  std::cout << "No query results for artist " << query.toLatin1().constData() << std::endl;
  QTimer::singleShot(100, this, SLOT(fill()));
}
