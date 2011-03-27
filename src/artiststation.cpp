#include <QList>
#include <QMap>
#include <QNetworkReply>

#include <ella/artist.h>

#include "artiststation.h"

ArtistStation::ArtistStation(const QString &name, QObject *parent)
 : QueryStation(name, parent)
{
}

ArtistStation::~ArtistStation()
{
}

void ArtistStation::search()
{
    QNetworkReply *reply = ella::Artist(name()).search();
    connect( reply, SIGNAL(finished()), SLOT(onGotSearch()));
}

void ArtistStation::onGotSearch()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() != QNetworkReply::NoError) {
        emit error(tr("This item is not available from streaming"));
        stop();
        return;
    }
    QList<ella::Artist> artists = ella::Artist::list(reply);
    if(artists.size() == 0) {
        emit error(tr("This item is not available from streaming"));
        stop();
        return;
    }
    QNetworkReply *reply2 = artists[0].getSimilar();
    connect(reply2, SIGNAL(finished()), this, SLOT(onGotSimilar()));
}

void ArtistStation::onGotSimilar()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() != QNetworkReply::NoError) {
        emit error(tr("This item is not available from streaming"));
        stop();
        return;
    }

    QMap<int, QString> similarArtists = ella::Artist::getSimilar(reply);
    createArtistSelector(similarArtists);
}
