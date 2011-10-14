#include <QNetworkReply>
#include <QMessageBox>
#include <QDebug>

#include <ella/track.h>
#include <ella/util.h>
#include <ella/ws.h>

#include "stationwidget.h"
#include "spotifysession.h"
#include "spotfmapp.h"
#include "radio.h"


StationWidget::StationWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    helloLabel->setText(QString(tr("Hello %1,")).arg(SpotifySession::self()->username()));
    connect(edit, SIGNAL(returnPressed()), this, SLOT(onClicked()));
    connect(edit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(playButton, SIGNAL(clicked()), this, SLOT(onClicked()));
    playButton->setEnabled(false);
    progressLabel->hide();
}


StationWidget::~StationWidget()
{
}

void StationWidget::setFocus()
{
    edit->setFocus();
}


void StationWidget::onTextChanged(const QString &text)
{
    playButton->setEnabled(!text.isEmpty());
}


void StationWidget::onClicked()
{
    edit->onCompleted();
    //Generate a new playlist
    QString artist = edit->text();
    if (artist.isEmpty())
        return;
   playButton->setEnabled(false);
   progressLabel->show();
   QNetworkReply *reply = ella::Track::getSimilar(
                   artist, ella::Track::SearchParams(),
                   ella::Util::Playlist);
   connect(reply, SIGNAL(finished()), this, SLOT(onGotSimilar()));
}


void StationWidget::onGotSimilar()
{
    try {
        QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
        QList<ella::Track> tracks = ella::Track::getSimilar(reply).values();
        SpotFm::app()->radio()->play(tracks);
    }
    catch(ella::ws::ParseError &e) {
        done();
        QMessageBox::critical(this, tr("Error"), tr("This item is not available for streaming"));
    }
}

void StationWidget::done()
{
    playButton->setEnabled(true);
    progressLabel->hide();
}


