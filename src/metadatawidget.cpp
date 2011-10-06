#include <QRegExp>
#include <QDebug>

#include "metadatawidget.h"
#include "albumimagefetcher.h"
#include "track.h"
#include "spotifysession.h"

MetadataWidget::MetadataWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    clear();
}

MetadataWidget::~MetadataWidget()
{
}

void MetadataWidget::clear()
{
    trackLabel->clear();
    artistLabel->clear();
    albumLabel->clear();
    bioLabel->clear();
    bioArtistLabel->clear();
    imageLabel->setPixmap(QPixmap::fromImage(QImage(":/icons/icons/no_cover.png")));
}

void MetadataWidget::setMetadata(const Track &track)
{
    trackLabel->setText(track.title());
    artistLabel->setText(QString(tr("by %1")).arg(track.artist()));
    albumLabel->setText(track.album());

    AlbumImageFetcher *aif = new AlbumImageFetcher(
        track.albumImage(SpotifySession::self()));
    connect(aif, SIGNAL(finished(QImage)), this, SLOT(onAlbumImage(QImage)));
    aif->fetch();

    bioArtistLabel->setText(track.artist());
    bioLabel->setText(QString(tr("loading...")));
    ArtistBiographyFetcher *abf = new ArtistBiographyFetcher(
            SpotifySession::self()->session(), track.spotifyArtist());
    connect(abf, SIGNAL(finished(QString)), this, SLOT(onArtistBio(QString)));
    abf->fetch();
}

void MetadataWidget::onAlbumImage(QImage image)
{
    imageLabel->setPixmap(QPixmap::fromImage(image));
    sender()->deleteLater();
}

void MetadataWidget::onArtistBio(QString bio)
{
    if (!bio.isEmpty()) { 
        bio = bio.remove(QRegExp("<[^>]*>"));
        bioLabel->setText(bio);
    }
    else
        bioLabel->setText(tr("No artist description available :("));
   
    sender()->deleteLater();
}

