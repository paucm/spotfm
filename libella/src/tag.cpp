#include <QNetworkReply>
#include <QtGlobal>

#include "xmlquery.h"
#include <ella/tag.h>
#include <ella/ws.h>

using namespace ella;

QNetworkReply* Tag::search(int limit) const
{
    QString path = "/collections/tags/tags/search";

    QMap<QString, QString> params;
    params["q"] = m_name;
    if (limit > 0) params["limit"] =  QString::number(limit);

    return ella::ws::get(path, params);
}

QMap<int, QString> Tag::list(QNetworkReply *reply)
{
    QMap<int, QString> tags;
    XmlQuery xml = ella::ws::parse(reply);
    float const max_score = xml["stats"]["max_score"].text().toFloat();
    Q_FOREACH(XmlQuery q, xml.children("tag")) {
        QString id = q.attribute("id");
        if (!id.isEmpty()) {
             int relevance = (q["relevance"].text().toFloat()*100)/max_score;
             tags.insertMulti(relevance, id);
        }
    }
    return tags;
}

QNetworkReply* Tag::getArtists() const
{
    QString tag = m_name;
    QString path = "/collections/tags/tags/" + tag.replace(' ', '+');
    path += "/similar/collections/bmat/artists";
    QMap<QString, QString> params;
    params["limit"] = "50";
    return ella::ws::get(path, params);
}
 
QMap<int, QString> Tag::getArtists(QNetworkReply *reply)
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
