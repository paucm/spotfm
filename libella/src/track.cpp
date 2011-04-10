#include <QNetworkReply>
#include <QLatin1String>

#include "xmlquery.h"
#include <ella/ws.h>
#include <ella/track.h>

using namespace ella;

Track::Track(const QByteArray &id, const QString &title, 
             const QByteArray &artistId, const QString &artistName)
{
    m_id = id;
    m_title = title;
    m_artistId = artistId;
    m_artistName = artistName;
}
            
Track::Track(const QByteArray &id)
{
    m_id = id;
}

QNetworkReply *Track::search(const SearchParams &params, int limit)
{
    QString query = searchParamsToQuery(params);
    return search(query, limit);
}
 
QNetworkReply *Track::search(const QString &query, int limit)
{
    QString path = "/collections/bmat/tracks/search";
    QMap<QString, QString> params;
    params["q"] = query;
    params["fetch_metadata"] = "track,artist,bmat_artist_id";
    if (limit > 0) params["limit"] =  QString::number(limit);

    return ella::ws::get(path, params);
}

QList<Track> Track::list(QNetworkReply *reply)
{
    QList<Track> tracks;
    XmlQuery xml = ella::ws::parse(reply);
    Q_FOREACH(XmlQuery q, xml.children("track")) {
        QString id = q.attribute("id");
        if (!id.isEmpty()) {
             XmlQuery ch = q.children("metadata")[0];
             QString title = ch["track"].text();
             QString artist = ch["artist"].text();
             QString artistId = ch["bmat_artist_id"].text();
             Track track(id.toLocal8Bit(), 
                         title, 
                         artistId.toLocal8Bit(), 
                         artist);
             tracks += track;
        }
    }
    return tracks;
}

QNetworkReply* Track::getSimilar(const SearchParams &params,
                                 Ella::SimilarityType type) const
{
    QString path = "/collections/bmat/tracks/" + m_id + "/similar/tracks";
    QMap<QString, QString> p;

    QString query = searchParamsToQuery(params);
    if (!query.isEmpty()) p["filter"] = query;

    QString t = Ella::similarityTypeToString(type);
    if (!t.isEmpty()) p["similarity_type"] = t;

    p["fetch_metadata"] = "track,artist,bmat_artist_id";
    p["limit"] = "50";

    return ella::ws::get(path, p);
}

QMap<int, Track> Track::getSimilar(QNetworkReply *reply)
{
    QMap<int, Track> tracks;
    XmlQuery xml = ella::ws::parse(reply);
    Q_FOREACH(XmlQuery q, xml.children("track")) {
        QString id = q.attribute("id");
        if (!id.isEmpty()) {
            int const relevance = q["relevance"].text().toFloat() * 100;
            XmlQuery ch = q.children("metadata")[0];
            QString title = ch["track"].text();
            QString artist = ch["artist"].text();
            QString artistId = ch["bmat_artist_id"].text();
            Track track(id.toLocal8Bit(),
                        title,
                        artistId.toLocal8Bit(),
                        artist);
            tracks.insertMulti(relevance, track);
        }
    }
    return tracks;
}

QString Track::searchParamsToQuery(const SearchParams &params)
{
    QString query;
    
    SearchParams::const_iterator iter = params.constBegin();
    for(; iter < params.constEnd(); ++iter) {
        if(!query.isEmpty()) {
            query += " AND ";
        }
        query += QLatin1String(searchParamToString(iter->first));
        query += ":";
        query += iter->second.toString();
    }
    return query;
}


QByteArray Track::searchParamToString(SearchParam param)
{
    switch(param) {
        case Title:
            return "title";
        case Artist:
            return "artist";
        case ArtistId:
            return "artist_id";
        case Genre:
            return "genre";
        case Speed:
            return "speed";
        case Mood:
            return "mood";
        default:
            return "";
    }
}

