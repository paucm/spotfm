#include <QInputDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "spotifysession.h"
#include "radio.h"
#include "station.h"
#include "track.h"
#include "albumimagefetcher.h"

MainWindow::MainWindow(QWidget *widget, Qt::WFlags fl)
  : QMainWindow(widget, fl)
  , Ui::MainWindow()

{
    setupUi(this);

    toogleButtons(false);
    Ui_MainWindow::mainToolBar->setIconSize(QSize(35, 35));

    SpotifySession *spSession = SpotifySession::self();

    connect(spSession, SIGNAL(loggedOut()), this, SLOT(onLoggedOut()));

    connect(actionPlay, SIGNAL(triggered()), this, SLOT(onPlay()));
    connect(actionSkip, SIGNAL(triggered()), this, SLOT(onSkip()));
    actionSkip->setEnabled(false);
    connect(actionPause, SIGNAL(triggered()), this, SLOT(onPause()));
    connect(actionStop, SIGNAL(triggered()), this, SLOT(onStop()));

    m_radio = 0;

    m_radio = new Radio();
    connect(m_radio, SIGNAL(playing(Track)), this, SLOT(onPlaying(Track)));
    connect(m_radio, SIGNAL(error(QString)), this, SLOT(onRadioError(QString)));
    connect(m_radio, SIGNAL(trackInQueue()), this, SLOT(enableSkipButton()));

    defaultWindow();
}

MainWindow::~MainWindow()
{
    if(m_radio) delete m_radio;
}

void MainWindow::defaultWindow()
{
    setWindowTitle("SpotFm");
    trackLabel->setText(QString());
    artistLabel->setText(QString());
    albumLabel->setText(QString());
    imageLabel->setPixmap(QPixmap::fromImage(QImage(":/icons/icons/no_cover.gif")));
    frame->setEnabled(false);
    toogleButtons(false);
}

void MainWindow::onLoggedOut()
{
    qDebug("logged out");
}

void MainWindow::onPlay()
{
    if (!SpotifySession::self()->isLoggedIn()) {
        QMessageBox::critical(this, tr("No logged in"), tr("First, you must be logged in in Spotify"));
        return;
    }
    
    if (m_radio->state() == Radio::Stopped) {

        actionPlay->setEnabled(false);
        QString station = customStation();
        if (!station.isEmpty()) {
            Station *st = new Station(station, this);
            m_radio->playStation(st);
        }
    }
    else if (m_radio->state() == Radio::Paused) {
        toogleButtons(true);
        m_radio->unpause();
   }
}

void MainWindow::onPlaying(const Track &track)
{
    toogleButtons(true);
    frame->setEnabled(true);
    Ui_MainWindow::statusBar->showMessage(
        QString(tr("%1 radio").arg(m_radio->station()->name())));
    QString title = track.title();
    QString artist = track.artist();
    setWindowTitle(QString(tr("%1 - %2").arg(artist).arg(title)));
    trackLabel->setText(title);
    artistLabel->setText(artist);
    albumLabel->setText(track.album());
    AlbumImageFetcher *aif = new AlbumImageFetcher(
        track.albumImage(SpotifySession::self()));
    connect(aif, SIGNAL(finished(QImage)), this, SLOT(onArtistImage(QImage)));
    aif->fetch();
}

void MainWindow::onArtistImage(QImage image)
{
    imageLabel->setPixmap(QPixmap::fromImage(image));
    sender()->deleteLater();
}

void MainWindow::onStop()
{
    m_radio->stopStation();
    toogleButtons(false);
    defaultWindow();
    Ui_MainWindow::statusBar->showMessage(QString());
}

void MainWindow::onPause()
{
    toogleButtons(false);
    m_radio->pause();
}

void MainWindow::onSkip()
{
    actionSkip->setEnabled(false);
    m_radio->skipTrack();
}

void MainWindow::onRadioError(const QString &msg)
{
    defaultWindow();
    QMessageBox::critical(this, tr("Error"), msg);
}

QString MainWindow::customStation()
{
    return QInputDialog::getText(
                      this,
                      tr("Create Custom Station"),
                      tr("Enter the name of a band or artist you like"));
}

void MainWindow::toogleButtons(bool enabled)
{
    actionPlay->setEnabled(!enabled);
    actionPause->setEnabled(enabled);
    actionStop->setEnabled(enabled);
}

void MainWindow::enableSkipButton()
{
    if (!actionSkip->isEnabled())
        actionSkip->setEnabled(true);
}
