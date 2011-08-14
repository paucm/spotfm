#ifndef ALBUM_IMAGE_FETCHER_H
#define ALBUM_IMAGE_FETCHER_H

#include <QObject>
#include <QImage>

#include <libspotify/api.h>

class AlbumImageFetcher : public QObject
{
    Q_OBJECT

    public:
        AlbumImageFetcher(sp_image *image);
        ~AlbumImageFetcher();

        void fetch();

    signals:
        void finished(QImage);

    private:
        static void SP_CALLCONV imageLoaded(sp_image *image, void *userdata);
        QImage readImage();
        sp_image *m_image;
};


class ArtistBiographyFetcher : public QObject
{
    Q_OBJECT

    public:
        ArtistBiographyFetcher(sp_session *session, sp_artist *artist);
        ~ArtistBiographyFetcher();

        void fetch();

    signals:
        void finished(QString);

    private:
        static void SP_CALLCONV artistBrowserLoaded(sp_artistbrowse *browse, void *userdata);
        sp_artist *m_artist;
        sp_session *m_session;
};


#endif

