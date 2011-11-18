#ifndef LASTFMHTTP_H
#define LASTFMHTTP_H

#include <QObject>
#include <QUrl>

class QNetworkAccessManager;


class LastFmHttp : public QObject
{
    Q_OBJECT
    public:
        LastFmHttp(QObject *parent=0);
        virtual ~LastFmHttp();

        void get(const QUrl &url);
        void put(const QUrl &url, QByteArray data);

    signals:
        void done(const QByteArray &response);

    private slots:
        void onFinished();

    protected:
        QNetworkAccessManager *m_nam;
};


#endif // LASTFMHTTP_H
