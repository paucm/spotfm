#ifndef ELLA_TRACK_H
#define ELLA_TRACK_H

#include <QString>
#include <QMap>
#include <QList>
#include <QPair>
#include <QByteArray>
#include <QVariant>

#include <ella/ella.h>

class QNetworkReply;

namespace ella {

    class Track
    {
        public:
             /** 
              * The following are the various search parameters to the search() and similar() functions
              *
              *  title      QString 
              *  artist     QString
              *  artist_id  QByteArray
              *  genre      QString 
              *  speed      QString [slow, medium, fast]
              *  mood       QString [blue, happy, furious, acoustic, party, relaxed]
              */

            enum SearchParam {
                Title,
                Artist,
                ArtistId,
                Genre,
                Speed,
                Mood,
            };
            typedef QPair<SearchParam, QVariant> SearchParamEntry;
            typedef QList<SearchParamEntry> SearchParams;

            Track(const QByteArray &id, const QString &title,
                  const QByteArray &artistId, const QString &artistName);
            Track(const QByteArray &id);

            QByteArray id() const { return m_id; }
            QString title() const { return m_title; }
            
            QByteArray artistId() const { return m_artistId; }
            QString artistName() const { return m_artistName; }

            static QNetworkReply *search(const SearchParams &params = SearchParams(), int limit=-1);
            static QNetworkReply *search(const QString &query, int limit=-1);
            static QList<Track> list(QNetworkReply *);

            QNetworkReply* getSimilar(const SearchParams &params = SearchParams(),
                                      Ella::SimilarityType type = Ella::Default) const;
            static QMap<int, Track> getSimilar(QNetworkReply *);

        private:
            static QString searchParamsToQuery(const SearchParams &params);
            static QByteArray searchParamToString(SearchParam param);

            QByteArray m_id;
            QString m_title;
            QByteArray m_artistId;
            QString m_artistName;
    };
}

#endif

