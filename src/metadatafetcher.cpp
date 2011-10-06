#include "metadatafetcher.h"

AlbumImageFetcher::AlbumImageFetcher(sp_image *image)
    : QThread()
{
    m_image = image;
    sp_image_add_ref(m_image);
}

AlbumImageFetcher::~AlbumImageFetcher()
{
    sp_image_release(m_image);
}

void AlbumImageFetcher::run()
{
    if (sp_image_is_loaded(m_image)) {
        QImage i = readImage();
        emit finished(i);
    }
    else {
        sp_image_add_load_callback(m_image, AlbumImageFetcher::imageLoaded, this);
    }
}

QImage AlbumImageFetcher::readImage()
{
    QImage i;
    size_t size;

    const void *buffer = sp_image_data(m_image, &size);
    i.loadFromData((const uchar *)buffer,size);
    i = i.scaled(200, 200);
    return i;
}

void SP_CALLCONV AlbumImageFetcher::imageLoaded(sp_image *image, void *userdata)
{
    AlbumImageFetcher *aif = static_cast<AlbumImageFetcher *>(userdata);

    sp_error error = sp_image_error(image);
    if (error != SP_ERROR_OK) {
        emit aif->finished(QImage(":/icons/icons/no_cover.gif"));
        return;
    }

    QImage i = aif->readImage();
    emit aif->finished(i);
}



ArtistBiographyFetcher::ArtistBiographyFetcher(sp_session *session, sp_artist *artist)
    : QThread()
{
    m_session = session;
    m_artist = artist;
}

ArtistBiographyFetcher::~ArtistBiographyFetcher()
{
}

void ArtistBiographyFetcher::run()
{
    sp_artistbrowse_create(m_session, m_artist, ArtistBiographyFetcher::artistBrowserLoaded, this);
}

void SP_CALLCONV ArtistBiographyFetcher::artistBrowserLoaded(sp_artistbrowse *browse, void *userdata)
{
    ArtistBiographyFetcher *abf = static_cast<ArtistBiographyFetcher *>(userdata);
    QString bio = QString::fromUtf8(sp_artistbrowse_biography(browse));
    //Get max 1000 characters
    int current = bio.indexOf(".");
    int pos = 0;
    while(current < 1000 && current != -1){
        pos = current;
        current = bio.indexOf(".", pos+1);
    }
    bio = bio.left(pos+1);
    emit abf->finished(bio);
}
