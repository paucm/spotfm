#ifndef ARTIST_IMAGE_FETCHER_H
#define ARTIST_IMAGE_FETCHER_H

#include <QObject>
#include <QImage>
#include <QNetworkAccessManager>

class QImage;

class ArtistImageFetcher : public QObject
{
    Q_OBJECT

    public:
        ArtistImageFetcher(const QString &artist) 
            :m_artist(artist)
        {
            m_ws = new QNetworkAccessManager(this);
        }

        void start();

    signals:
        void finished(QImage);
    
    private slots:
        void onArtistGotInfo();
        void onArtistImageDownloaded();

    private:
        void fail();
        bool downloadImage(const QByteArray &xml);
        void getSummary(const QByteArray &xml);

        QString m_artist;
        QNetworkAccessManager *m_ws;
};

#endif

