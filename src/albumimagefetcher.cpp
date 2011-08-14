#include "albumimagefetcher.h"

AlbumImageFetcher::AlbumImageFetcher(sp_image *image)
{
    m_image = image;
    sp_image_add_ref(m_image);
}

AlbumImageFetcher::~AlbumImageFetcher()
{
    sp_image_release(m_image);
}

void AlbumImageFetcher::fetch()
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
{
    m_session = session;
    m_artist = artist;
}

ArtistBiographyFetcher::~ArtistBiographyFetcher()
{
}

void ArtistBiographyFetcher::fetch()
{
    sp_artistbrowse_create(m_session, m_artist, ArtistBiographyFetcher::artistBrowserLoaded, this);
}

void SP_CALLCONV ArtistBiographyFetcher::artistBrowserLoaded(sp_artistbrowse *browse, void *userdata)
{
    ArtistBiographyFetcher *abf = static_cast<ArtistBiographyFetcher *>(userdata);
    QString bio = sp_artistbrowse_biography(browse);
    //Get first paragraph
    int pos = bio.indexOf("\n");
    if (pos > 0)
        bio = bio.left(pos);
    emit abf->finished(bio);
}
