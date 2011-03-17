#include <QNetworkReply>
#include <QtGlobal>

#include "xmlquery.h"
#include <ella/artist.h>
#include <ella/ws.h>

using namespace ella;

QNetworkReply* Artist::search(int limit) const
{
    QString path = "/collections/bmat/artists/search";

    QMap<QString, QString> params;
    params["q"] = m_name;
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
    QString path = "/collections/bmat/artists/" + m_bmatid + "/similar/artists";
    QMap<QString, QString> params;
    params["limit"] = "100";
    return ella::ws::get(path, params);
}
 
QMap<int, QString> Artist::getSimilar(QNetworkReply *reply)
{
     QMap<int, QString> artists;
     XmlQuery xml = ella::ws::parse(reply);
     Q_FOREACH(XmlQuery q, xml.children("artist")) {
         QString id = q.attribute("id");
         if (!id.isEmpty()) {
             int const relevance = q["relevance"].text().toFloat() * 100;
             QString name = q["metadata"]["artist"].text();
             artists.insertMulti(relevance, name);
         }
     }
     return artists;
}

