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
    i.loadFromData((const uchar *)buffer, size);
    return i;
}

void AlbumImageFetcher::imageLoaded(sp_image *image, void *userdata)
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
