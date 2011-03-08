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

    connect(actionPlay, SIGNAL(triggered()), this, SLOT(onPlay()));
    connect(actionSkip, SIGNAL(triggered()), this, SLOT(onSkip()));
    actionSkip->setEnabled(false);
    connect(actionPause, SIGNAL(triggered()), this, SLOT(onPause()));
    connect(actionStop, SIGNAL(triggered()), this, SLOT(onStop()));

    connect(actionLogoutAndQuit, SIGNAL(triggered()), qApp, SLOT(logoutAndQuit()));
    connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(logout()));

    m_radio = 0;

    m_radio = new Radio();
    connect(m_radio, SIGNAL(playing(Track)), this, SLOT(onPlaying(Track)));
    connect(m_radio, SIGNAL(error(QString)), this, SLOT(onRadioError(QString)));
    connect(m_radio, SIGNAL(trackInQueue()), this, SLOT(enableSkipButton()));
    connect(m_radio, SIGNAL(trackProgress(int)), this, SLOT(onTrackProgress(int)));

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
    timeLabel->setText(QString(tr("--:--")));
    totalTimeLabel->setText(QString(tr("--:--")));
    slider->setValue(0);
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
        actionPlay->setEnabled(true);
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
    
    int duration = track.duration();
    QString total = QString("%1:%2")
        .arg((duration / 1000) / 60, 2, 10, QLatin1Char('0'))
        .arg((duration / 1000) % 60, 2, 10, QLatin1Char('0'));
    totalTimeLabel->setText(total);
    timeLabel->setText(QString(tr("00:00")));
    //slider->setRange(0, duration);
    slider->setMinimum(0);
    slider->setMaximum(duration);
    slider->setValue(0);

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

void MainWindow::onTrackProgress(int pos)
{
    slider->setValue(pos);

    QString progress = QString("%1:%2")
        .arg((slider->value() / 1000) / 60, 2, 10, QLatin1Char('0'))
        .arg((slider->value() / 1000) % 60, 2, 10, QLatin1Char('0'));
    timeLabel->setText(progress);
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
