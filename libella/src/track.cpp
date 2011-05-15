#include <QNetworkReply>
#include <QLatin1String>

#include <ella/ws.h>
#include <ella/track.h>

using namespace ella;

static QString fetch_metadata = "track,artist,bmat_artist_id,"
                                "highlevel_moods_happy_probability_value,"
                                "highlevel_moods_acoustic_probability_value,"
                                "highlevel_moods_aggressive_probability_value,"
                                "highlevel_moods_relaxed_probability_value,"
                                "highlevel_moods_party_probability_value,"
                                "highlevel_moods_sad_probability_value,"
                                "rhythm_bpm_value,year,genre,track_genre";


Track::Track(const QByteArray &id, const QString &title,
             const QByteArray &artistId, const QString &artistName)
{
    m_id = id;
    m_title = title;
    m_artistId = artistId;
    m_artistName = artistName;
    m_bpm = 0;
    m_year = 0;
}

Track::Track(const QByteArray &id)
{
    m_id = id;
    m_bpm = 0;
    m_year = 0;
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
    params["fetch_metadata"] = fetch_metadata;
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
    QString path = "/collections/bmat/artists/" + artist.id() +
                   "/similar/tracks";
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

    QMap<int, Util::Mood> moods;
    QString value = xml["highlevel_moods_happy_probability_value"].text();
    float v = value.remove(0, 1).toFloat() * 100;
    moods.insertMulti(v, Util::Happy);

    value = xml["highlevel_moods_acoustic_probability_value"].text();
    v = value.remove(0, 1).toFloat() * 100;
    moods.insertMulti(v, Util::Acoustic);

    value = xml["highlevel_moods_aggressive_probability_value"].text();
    v = value.remove(0, 1).toFloat() * 100;
    moods.insertMulti(v, Util::Furious);

    value = xml["highlevel_moods_relaxed_probability_value"].text();
    v = value.remove(0, 1).toFloat() * 100;
    moods.insertMulti(v, Util::Relax);

    value = xml["highlevel_moods_party_probability_value"].text();
    v = value.remove(0, 1).toFloat() * 100;
    moods.insertMulti(v, Util::Party);

    value = xml["highlevel_moods_sad_probability_value"].text();
    v = value.remove(0, 1).toFloat() * 100;
    moods.insertMulti(v, Util::Blue);

    track.m_moods = moods;

    value = xml["rhythm_bpm_value"].text();
    v = value.remove(0, 1).toFloat();
    track.m_bpm = v;

    value = xml["year"].text();
    if (!value.isEmpty())
        track.m_year = value.toInt();

    Q_FOREACH(XmlQuery q, xml.children("genre")) {
        if (!track.m_genres.contains(q.text())) {
            track.m_genres << q.text();
        }
    }

    Q_FOREACH(XmlQuery q, xml.children("track_genre")) {
        if (!track.m_genres.contains(q.text())) {
            track.m_genres << q.text();
        }
    }
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

