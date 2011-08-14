#ifndef METADATAWIDGET_H
#define METADATAWIDGET_H

#include <QWidget>
#include <ui_metadatawidget.h>

class Track;

class MetadataWidget : public QWidget, private Ui::MetadataWidget
{
    Q_OBJECT

    public:
        MetadataWidget(QWidget *widget=0);
        ~MetadataWidget();

        //Show the not playing screen
        void clear();
        void setMetadata(const Track &track);

    private slots:
        void onAlbumImage(QImage);
        void onArtistBio(QString);
};

#endif

