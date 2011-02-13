#include <QNetworkReply>
#include <QRegExp>
#include "artistimagefetcher.h"
#include <lastfm.h>

void ArtistImageFetcher::start()
{
    QNetworkReply* reply = lastfm::Artist(m_artist).getInfo();
    connect(reply, SIGNAL(finished()), this, SLOT(onArtistGotInfo()));                
}

void ArtistImageFetcher::onArtistGotInfo()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    QByteArray xml = reply->readAll();
    if(!downloadImage(xml))
        fail();
    reply->deleteLater();
}

void ArtistImageFetcher::onArtistImageDownloaded()
{
    QImage i;
    i.loadFromData(((QNetworkReply*)sender())->readAll());
    if (!i.isNull()) {
        i.scaled(137, 137);
        emit finished(i);
    }
    else
        fail();
    
    sender()->deleteLater(); //always deleteLater from slots connected to sender()*/
}

bool ArtistImageFetcher::downloadImage(const QByteArray &xml)
{ 
    lastfm::XmlQuery xq(xml);
    foreach (QString size, QStringList() << "extralarge" << "large")
    {
        try {
            QUrl url(xq["artist"]["image size="+size].text());
            if (!url.isValid())
                return false;
            
            QNetworkReply* get = m_ws->get( QNetworkRequest( url ) );
            connect( get, SIGNAL(finished()), SLOT(onArtistImageDownloaded()) );
            return true;
        }
        catch (std::runtime_error& e)
        {}
    }
    return false;
}

void ArtistImageFetcher::fail()
{
    emit finished(QImage(":/icons/icons/no_cover.gif"));
}

