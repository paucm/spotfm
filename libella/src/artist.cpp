#include <QNetworkReply>
#include <QtGlobal>

#include "xmlquery.h"
#include <ella/artist.h>
#include <ella/ws.h>

using namespace ella;

QNetworkReply* Artist::search(const QString &query, int limit)
{
    QString path = "/collections/bmat/artists/search";

    QMap<QString, QString> params;
    params["q"] = query;
    params["fetch_metadata"] = "artist";
    if (limit > 0) params["limit"] =  QString::number(limit);

    return ella::ws::get(path, params);
}

QList<Artist> Artist::list(QNetworkReply *reply)
{
    QList<Artist> artists;
    XmlQuery xml = ella::ws::parse(reply);
    Q_FOREACH(XmlQuery q, xml.children("artist")) {
        QString id = q.attribute("id");
        if (!id.isEmpty()) {
             XmlQuery ch = q.children("metadata")[0];
             QString name = ch["artist"].text();
             Artist artist(id.toLocal8Bit(), name);
             artists += artist;
        }
    }
    return artists;
}

QNetworkReply* Artist::getSimilar() const
{
    QString path = "/collections/bmat/artists/" + m_id + "/similar/artists";
    QMap<QString, QString> params;
    params["limit"] = "50";
    return ella::ws::get(path, params);
}
 
QMap<int, Artist> Artist::getSimilar(QNetworkReply *reply)
{
     QMap<int, Artist> artists;
     XmlQuery xml = ella::ws::parse(reply);
     Q_FOREACH(XmlQuery q, xml.children("artist")) {
         QString id = q.attribute("id");
         if (!id.isEmpty()) {
             int const relevance = q["relevance"].text().toFloat() * 100;
             QString name = q["metadata"]["artist"].text();
             artists.insertMulti(relevance, Artist(id.toLocal8Bit(), name));
         }
     }
     return artists;
}

QNetworkReply* Artist::getSimilarTracks(Ella::SimilarityType type) const
{
    QString path = "/collections/bmat/artists/" + m_id + "/similar/tracks";
    QMap<QString, QString> p;

    QString t = Ella::similarityTypeToString(type);
    if (!t.isEmpty()) p["similarity_type"] = t;
    p["fetch_metadata"] = "track,artist,bmat_artist_id";
    p["limit"] = "50";

    return ella::ws::get(path, p);
}

QMap<int, Track> Artist::getSimilarTracks(QNetworkReply *reply)
{
    return Track::getSimilar(reply);
}
