#include <QNetworkReply>
#include <QLatin1String>

#include <ella/ws.h>
#include <ella/track.h>

using namespace ella;

static QString fetch_metadata = "track,artist,bmat_artist_id";


Track::Track()
{
    m_id = "";
}

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

QNetworkReply *Track::search(const SearchParams &params, int offset, int limit)
{
    QString query = searchParamsToQuery(params);
    return search(query, offset, limit);
}

QNetworkReply *Track::search(const QString &query, int offset, int limit)
{
    QString path = "/collections/bmat/tracks/search";
    QMap<QString, QString> params;
    params["q"] = query;
    params["fetch_metadata"] = fetch_metadata;
    params["offset"] = QString::number(offset);
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
            Track track(id.toLocal8Bit());
            XmlQuery ch = q.children("metadata")[0];
            parseMetadata(ch, track);
            tracks += track;
        }
    }
    return tracks;
}

QNetworkReply* Track::getSimilar(const SearchParams &params,
                                 Util::SimilarityType type) const
{
    QString path = "/collections/bmat/tracks/" + m_id + "/similar/tracks";
    QMap<QString, QString> p;

    QString query = searchParamsToQuery(params);
    if (!query.isEmpty()) p["filter"] = query;

    QString t = Util::similarityTypeToString(type);
    if (!t.isEmpty()) p["similarity_type"] = t;

    p["fetch_metadata"] = fetch_metadata;
    p["limit"] = "50";

    return ella::ws::get(path, p);
}

QNetworkReply* Track::getSimilar(const Artist &artist,
                                 const SearchParams &params,
                                 Util::SimilarityType type)
{
    QString path = "/collections/bmat/artists/";
    QString q;
    if (!artist.id().isEmpty())
        q = artist.id();
    else if (!artist.name().isEmpty())
        q = artist.name();
    path += q + "/similar/tracks";
    QMap<QString, QString> p;

    QString query = searchParamsToQuery(params);
    if (!query.isEmpty()) p["filter"] = query;

    QString t = Util::similarityTypeToString(type);
    if (!t.isEmpty()) p["similarity_type"] = t;

    p["fetch_metadata"] = fetch_metadata;
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
            Track track(id.toLocal8Bit());
            XmlQuery ch = q.children("metadata")[0];
            parseMetadata(ch, track);
            tracks.insertMulti(relevance, track);
        }
    }
    return tracks;
}

void Track::parseMetadata(XmlQuery xml, Track &track)
{
    track.m_title = xml["track"].text();
    track.m_artistName = xml["artist"].text();
    track.m_artistId = xml["bmat_artist_id"].text().toLocal8Bit();
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
        if (iter->first == Mood)
            query += Util::moodToString(static_cast<Util::Mood>(iter->second.toInt()));
        else if(iter->first == Speed)
            query += Util::speedToString(static_cast<Util::Speed>(iter->second.toInt()));
        else
            query += iter->second.toString();
    }
    return query;
}

QByteArray Track::searchParamToString(SearchParam param)
{
    switch(param) {
        case Title:
            return "title";
        case ArtistName:
            return "artist";
        case ArtistId:
            return "artist_id";
        case Genre:
            return "track_genre";
        case Speed:
            return "speed";
        case Mood:
            return "mood";
        default:
            return "";
    }
}

