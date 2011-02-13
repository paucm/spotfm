#include <QInputDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "spotifysession.h"
#include "radio.h"
#include "station.h"
#include "track.h"
#include "artistimagefetcher.h"

MainWindow::MainWindow(QWidget *widget, Qt::WFlags fl)
  : QMainWindow(widget, fl)
  , Ui::MainWindow()

{
    setupUi(this);

    toogleButtons(false);
    Ui_MainWindow::mainToolBar->setIconSize(QSize(35, 35));

    SpotifySession *spSession = new SpotifySession();

    connect(spSession, SIGNAL(loggedError(QString)), this, SLOT(onLoggedError(QString)));
    connect(spSession, SIGNAL(loggedIn()), this, SLOT(onLoggedIn()));
    connect(spSession, SIGNAL(loggedOut()), this, SLOT(onLoggedOut()));

    connect(actionPlay, SIGNAL(triggered()), this, SLOT(onPlay()));
    connect(actionSkip, SIGNAL(triggered()), this, SLOT(onSkip()));
    connect(actionPause, SIGNAL(triggered()), this, SLOT(onPause()));
    connect(actionStop, SIGNAL(triggered()), this, SLOT(onStop()));

    m_radio = 0;

    defaultWindow();
}

MainWindow::~MainWindow()
{
    if(m_radio) delete m_radio;
    delete SpotifySession::self();
}

void MainWindow::defaultWindow()
{
    trackLabel->setText(QString());
    artistLabel->setText(QString());
    albumLabel->setText(QString());
    imageLabel->setPixmap(QPixmap::fromImage(QImage(":/icons/icons/no_cover.gif")));
    frame->setEnabled(false);
}

void MainWindow::onLoggedError(const QString &msg)
{
    Ui_MainWindow::statusBar->showMessage(QString(tr("Loggin error: %1")).arg(msg));
}
    
void MainWindow::onLoggedIn()
{
    Ui_MainWindow::statusBar->showMessage(tr("Logged in"));
    m_radio = new Radio();
    connect(m_radio, SIGNAL(playing(Track)), this, SLOT(onPlaying(Track)));
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
        Ui_MainWindow::statusBar->showMessage(QString(tr("Playing %1's station").arg(station)));
        Station *st = new Station(station, this);
        m_radio->playStation(st);
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
    trackLabel->setText(track.title());
    artistLabel->setText(track.artist());
    albumLabel->setText(track.album());
    ArtistImageFetcher *aif = new ArtistImageFetcher(track.artist());
    connect(aif, SIGNAL(finished(QImage)), this, SLOT(onArtistImage(QImage)));
    aif->start();
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
    actionSkip->setEnabled(enabled);
    actionPause->setEnabled(enabled);
    actionStop->setEnabled(enabled);
}
