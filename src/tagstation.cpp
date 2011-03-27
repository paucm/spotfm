#include <QMap>
#include <QNetworkReply>

#include <ella/tag.h>

#include "tagstation.h"

TagStation::TagStation(const QString &name, QObject *parent)
    : QueryStation(name, parent)
{
}

TagStation::~TagStation()
{
}

void TagStation::search()
{
    QNetworkReply *reply = ella::Tag(name()).getArtists();
    connect( reply, SIGNAL(finished()), SLOT(onGotSearch()));
}

void TagStation::onGotSearch()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() != QNetworkReply::NoError) {
        emit error(tr("This item is not available from streaming"));
        stop();
        return;
    }

    QMap<int, QString> artists = ella::Tag::getArtists(reply);
    createArtistSelector(artists);
}
