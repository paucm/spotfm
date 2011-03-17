#ifndef STATION_H
#define STATION_H

#include <QObject>
#include <QString>
#include <QStringList>

#include <libspotify/api.h>

#include "track.h"

class SpotifyQuery;
class QTimer;
class Selector;

class Station : public QObject {
  Q_OBJECT

  public:
    Station(const QString &name, QObject *parent=0);
    ~Station();

    QString name() const { return m_name; }
    Track takeNextTrack();

    void start() { m_stop = false; fill(); }
    void stop();

  signals:
    void trackAvailable();
    void noArtistFound();

  private slots:
    void fill();
    void onQueryCompleted(const Track &t);
    void onQueryError(const QString &q, const QString &msg);
    void onQueryNoResults(const QString &q);

    void onGotSearch();

    void onMetadataUpdated();

  private:
    void search();

  private:
    QString m_name;
    bool m_stop;
    SpotifyQuery *m_sp_query;

    Selector *m_artistSelector;
    QStringList m_artistHistory;
    QList<Track> m_queue;
    QList<Track> m_pending;

    QTimer *m_timer;
};

#endif
