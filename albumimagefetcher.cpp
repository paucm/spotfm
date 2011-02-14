#include "albumimagefetcher.h"

AlbumImageFetcher::AlbumImageFetcher(sp_image *image)
{
    m_image = image;
    sp_image_add_ref(m_image);
    sp_image_add_load_callback(m_image, AlbumImageFetcher::imageLoaded, this);
}

AlbumImageFetcher::~AlbumImageFetcher()
{
    sp_image_remove_load_callback(m_image, AlbumImageFetcher::imageLoaded, this);
    sp_image_release(m_image);
}

void AlbumImageFetcher::imageLoaded(sp_image *image, void *userdata)
{
    QImage i;
    size_t size;
    AlbumImageFetcher *aif = static_cast<AlbumImageFetcher *>(userdata);

    const void *buffer = sp_image_data(image, &size);
    i.loadFromData((const uchar *)buffer, size);
    emit aif->finished(i);
}
