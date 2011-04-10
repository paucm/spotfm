#ifndef ELLA_ARTIST_H
#define ELLA_ARTIST_H

#include <QString>
#include <QMap>
#include <QList>

#include <ella/ella.h>
#include <ella/track.h>

class QNetworkReply;

namespace ella {

    class Artist
    {
        public:
            Artist(const QByteArray &id, const QString &name)
                : m_id(id)
                , m_name(name)
            {
            }

            Artist(const QByteArray &id)
                : m_id(id)
            {
            }

            QString name() const
            {
                return m_name;
            }

            QString bmatid() const
            {
                return m_id;
            }

            static QNetworkReply* search(const QString &query, int limit = -1);
            static QList<Artist> list(QNetworkReply *);

            QNetworkReply* getSimilar() const;
            static QMap<int, Artist> getSimilar(QNetworkReply *);

            QNetworkReply* getSimilarTracks(Ella::SimilarityType type = Ella::Default) const;
            static QMap<int, Track> getSimilarTracks(QNetworkReply *);

        private:
            QString m_id;
            QString m_name;
    };
}

#endif
