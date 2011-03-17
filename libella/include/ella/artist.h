#ifndef ELLA_ARTIST_H
#define ELLA_ARTIST_H

#include <QString>
#include <QMap>
#include <QList>

class QNetworkReply;

namespace ella {

    class Artist
    {
        public:
            Artist(const QByteArray &bmatid, const QString &name)
                : m_bmatid(bmatid)
                , m_name(name)
            {
            }

            Artist(const QByteArray &bmatid)
                : m_bmatid(bmatid)
            {
            }

            Artist(const QString &name)
                : m_name(name)
            {
            }

            QString name() const
            {
                return m_name;
            }

            QString bmatid() const
            {
                return m_bmatid;
            }

            QNetworkReply* search( int limit = -1 ) const;
            static QList<Artist> list(QNetworkReply *);

            QNetworkReply* getSimilar() const;
            static QMap<int, QString> getSimilar(QNetworkReply *);

        private:
            QString m_bmatid;
            QString m_name;
    };
}

#endif
