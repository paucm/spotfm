#ifndef ELLA_TRACK_H
#define ELLA_TRACK_H

#include <QString>
#include <QMap>
#include <QList>
#include <QPair>
#include <QByteArray>
#include <QVariant>
#include <QStringList>

#include <ella/util.h>
#include <ella/artist.h>
#include <ella/xmlquery.h>

class QNetworkReply;

namespace ella {

    class Track
    {
        public:
             /**
              * The following are the various search parameters to the search() and similar() functions
              *
              *  Title      QString
              *  ArtistName QString
              *  ArtistId   QByteArray
              *  Genre      QString
              *  Speed      Util::Speed
              *  Mood       Util::Mood
              */

            enum SearchParam {
                Title,
                ArtistName,
                ArtistId,
                Genre,
                Speed,
                Mood,
            };
            typedef QPair<SearchParam, QVariant> SearchParamEntry;
            typedef QList<SearchParamEntry> SearchParams;

            Track();
            Track(const QByteArray &id, const QString &title,
                  const QByteArray &artistId, const QString &artistName);
            Track(const QByteArray &id);

            QByteArray id() const { return m_id; }
            QString title() const { return m_title; }

            QByteArray artistId() const { return m_artistId; }
            QString artistName() const { return m_artistName; }

            int bpm() const { return m_bpm; }
            int year() const { return m_year; }
            QStringList genres() const { return m_genres; }

            static QNetworkReply *search(
                    const SearchParams &params = SearchParams(),
                    int offset=0,
                    int limit=-1);
            static QNetworkReply *search(const QString &query, int offset=0, int limit=-1);
            static QList<Track> list(QNetworkReply *);

            static QNetworkReply* getSimilar(
                    const Artist &artist,
                    const SearchParams &params = SearchParams(),
                    Util::SimilarityType type = Util::Default);
            QNetworkReply* getSimilar(
                    const SearchParams &params = SearchParams(),
                    Util::SimilarityType type = Util::Default) const;
            static QMap<int, Track> getSimilar(QNetworkReply *);

        private:
            static QString searchParamsToQuery(const SearchParams &params);
            static QByteArray searchParamToString(SearchParam param);
            static void parseMetadata(XmlQuery xml, Track &track);

            QByteArray m_id;
            QString m_title;
            QByteArray m_artistId;
            QString m_artistName;
            int m_bpm;
            int m_year;
            QStringList m_genres;
    };
}

#endif

