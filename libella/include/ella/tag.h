#ifndef ELLA_TAG_H
#define ELLA_TAG_H

#include <QString>
#include <QMap>
#include <QList>

#include <ella/artist.h>

class QNetworkReply;

namespace ella {

    class Tag
    {
        public:
            Tag(const QString &name)
                : m_name(name)
            {
            }

            QString name() const
            {
                return m_name;
            }

            QNetworkReply* search( int limit = -1 ) const;
            static QMap<int, QString> list(QNetworkReply *);

            QNetworkReply* getArtists() const;
            static QMap<int, Artist> getArtists(QNetworkReply *);

        private:
            QString m_name;
    };
}

#endif
