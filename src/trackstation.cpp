#include <QList>
#include <QMap>
#include <QNetworkReply>

#include <ella/track.h>

#include "trackstation.h"

TrackStation::TrackStation(const QString &name, QObject *parent)
 : QueryStation(name, parent)
{
}

TrackStation::~TrackStation()
{
}

void TrackStation::search()
{
    QNetworkReply *reply = ella::Track::search(name());
    connect( reply, SIGNAL(finished()), SLOT(onGotSearch()));
}

void TrackStation::onGotSearch()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() != QNetworkReply::NoError) {
        emit error(tr("This item is not available from streaming"));
        stop();
        return;
    }
    QList<ella::Track> tracks = ella::Track::list(reply);
    if(tracks.size() == 0) {
        emit error(tr("This item is not available from streaming"));
        stop();
        return;
    }
    QNetworkReply *reply2 = tracks[0].getSimilar(ella::Track::SearchParams(), ella::Ella::Playlist);
    connect(reply2, SIGNAL(finished()), this, SLOT(onGotSimilar()));
}

void TrackStation::onGotSimilar()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() != QNetworkReply::NoError) {
        emit error(tr("This item is not available from streaming"));
        stop();
        return;
    }

    QList<ella::Track> tracks = ella::Track::getSimilar (reply).values();
    setTracks(tracks);
}
