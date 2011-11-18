#ifndef LASTFMSERVICE_H
#define LASTFMSERVICE_H

#include <QObject>
#include <QString>
#include "track.h"


class QNetworkAccessManager;
class QTimer;


class LastFmService : public QObject
{
    Q_OBJECT

    public:
        LastFmService(const QString &username, const QString &password, const QString &sessionKey, QObject *parent=0);
        ~LastFmService();

        void authenticate();

        QString sessionKey() const { return m_sessionKey; }

    signals:
        void authenticated();

    public slots:
        void nowPlaying(const Track &track);
        void scrobble(int at);

    private slots:
        void requestNowPlaying();
        void onAuthenticated();
        void onNowPlaying();
        void onScrobble();

    private:
        QString m_username;
        QString m_password;
        QString m_sessionKey;
        Track m_currentTrack;

        QNetworkAccessManager *m_nam;

        QTimer *m_timer;

        static QString apiKey;
        static QString secret;

        static const int defaultScrobblePoint = 50;
        static const int scrobbleMinDuration = 31;
        static const int scrobbleMaxDuration = 240;


};

#endif
