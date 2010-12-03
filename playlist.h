#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <QString>
#include <QStringList>

#include <libspotify/api.h>

#include "track.h"

class SpotifyQuery;


class Playlist : public QObject {
  Q_OBJECT
  
  public:
    Playlist(const QString &name, sp_session *session);
    ~Playlist();
   
    QString name() const { return m_name; }
    Track takeNextTrack();
    
  public slots:
    void fill();
    
  signals:
    void trackAvailable();
    
  private slots:
    void onQueryCompleted(const Track &t);
    void onQueryError(const QString &q, const QString &msg);
    void onQueryNoResults(const QString &q);
    
    void onGotSearch();
    
    void onMetadataUpdated();
    
  private:
    void search();
    
  private:
    QString m_name;
    SpotifyQuery *m_sp_query;
    
    QStringList m_artists;
    QStringList m_artistHistory;
    QList<Track> m_queue;
    QList<Track> m_pending;
    
};

#endif
