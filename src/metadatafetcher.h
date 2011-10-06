#ifndef ALBUM_IMAGE_FETCHER_H
#define ALBUM_IMAGE_FETCHER_H

#include <QObject>
#include <QImage>
#include <QThread>

#include <libspotify/api.h>

class AlbumImageFetcher : public QThread
{
    Q_OBJECT

    public:
        AlbumImageFetcher(sp_image *image);
        ~AlbumImageFetcher();

        void run();

    signals:
        void finished(QImage);

    private:
        static void SP_CALLCONV imageLoaded(sp_image *image, void *userdata);
        QImage readImage();
        sp_image *m_image;
};


class ArtistBiographyFetcher : public QThread
{
    Q_OBJECT

    public:
        ArtistBiographyFetcher(sp_session *session, sp_artist *artist);
        ~ArtistBiographyFetcher();

        void run();

    signals:
        void finished(QString);

    private:
        static void SP_CALLCONV artistBrowserLoaded(sp_artistbrowse *browse, void *userdata);
        static void SP_CALLCONV imageLoaded(sp_image *image, void *userdata);

        sp_artist *m_artist;
        sp_session *m_session;
};

#endif
