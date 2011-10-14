#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QCloseEvent>
#include <QMap>
#include <QMapIterator>
#include <QSettings>

#include <ella/util.h>

#include "spotfmapp.h"
#include "radio.h"
#include "mainwindow.h"
#include "spotifysession.h"
#include "track.h"
#include "aboutdialog.h"
#include "metadatawidget.h"

#define TAB_STATION 0
#define TAB_PLAYING 1

MainWindow::MainWindow(QWidget *widget, Qt::WFlags fl)
  : QMainWindow(widget, fl)
  , Ui::MainWindow()
{
    setupUi(this);

    Ui_MainWindow::mainToolBar->setIconSize(QSize(35, 35));

    connect(actionPlay, SIGNAL(triggered()), this, SLOT(onPlay()));
    connect(actionSkip, SIGNAL(triggered()), this, SLOT(onSkip()));
    connect(actionPause, SIGNAL(triggered()), this, SLOT(onPause()));
    connect(actionStop, SIGNAL(triggered()), this, SLOT(onStop()));
    connect(actionVolumeUp, SIGNAL(triggered()), this, SLOT(onVolumeUp()));
    connect(actionVolumeDown, SIGNAL(triggered()), this, SLOT(onVolumeDown()));
    connect(actionMute, SIGNAL(triggered()), this, SLOT(onMute()));

    connect(actionLogoutAndQuit, SIGNAL(triggered()), qApp, SLOT(logoutAndQuit()));
    connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(logout()));

    AboutDialog *about = new AboutDialog(this);
    connect(actionAbout, SIGNAL(triggered()), about, SLOT(show()));

    Radio *radio = SpotFm::app()->radio();
    connect(radio, SIGNAL(playing(Track)), this, SLOT(onPlaying(Track)));
    connect(radio, SIGNAL(error(QString)), this, SLOT(onRadioError(QString)));
    connect(radio, SIGNAL(trackInQueue()), this, SLOT(enableSkipButton()));
    connect(radio, SIGNAL(trackProgress(int)), this, SLOT(onTrackProgress(int)));
    connect(volumeSlider, SIGNAL(valueChanged(int)), radio, SLOT(setVolume(int)));

    defaultWindow();
    setupTrayIcon();

    stationWidget->setFocus();

    QSettings *s = SpotFm::app()->settings();
    volumeSlider->setValue(s->value("volume", 80).toInt());
    radio->setVolume(volumeSlider->value());
    m_lastVolume = volumeSlider->value();
}

MainWindow::~MainWindow()
{
    QSettings *s = SpotFm::app()->settings();
    s->setValue("volume", volumeSlider->value());
}

void MainWindow::defaultWindow()
{
    setWindowTitle("SpotFm");
    actionPlay->setEnabled(false);
    actionPause->setEnabled(false);
    actionStop->setEnabled(false);
    actionSkip->setEnabled(false);
    timeLabel->setText(QString(tr("--:--")));
    totalTimeLabel->setText(QString(tr("--:--")));
    slider->setValue(0);
    tabWidget->setCurrentIndex(TAB_STATION);
    metadataWidget->clear();
    stationWidget->done();
}

void MainWindow::setupTrayIcon()
{
    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Open"), this, SLOT(restoreWindow()));
    menu->addSeparator();
    menu->addAction(actionPlay);
    menu->addAction(actionPause);
    menu->addAction(actionSkip);
    menu->addSeparator();
    menu->addAction(actionQuit);

    m_trayIcon = new QSystemTrayIcon( this );
    m_trayIcon->setContextMenu(menu);
    m_trayIcon->setIcon(QPixmap::fromImage(QImage(":/icons/icons/spotfm.png")));

    connect(m_trayIcon, SIGNAL(activated( QSystemTrayIcon::ActivationReason)),
            this, SLOT(onTrayIconActivated( QSystemTrayIcon::ActivationReason)));

    m_trayIcon->show();
}

void MainWindow::restoreWindow()
{
    showNormal();
    activateWindow();
    raise();
}

void MainWindow::onTrayIconActivated( QSystemTrayIcon::ActivationReason reason )
{
    // typical linux behavior is single clicking tray icon toggles the main window
    #ifdef Q_WS_X11
    if (reason == QSystemTrayIcon::Trigger)
        toggleWindowVisibility();
    #else
    if (reason == QSystemTrayIcon::DoubleClick)
        toggleWindowVisibility();
    #endif
}

void MainWindow::toggleWindowVisibility()
{
    if(isVisible()) {
        hide();
    }
    else {
        show();
        activateWindow();
        raise();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}


void MainWindow::onPlaying(const Track &track)
{
    tooglePlayPauseButtons(true);
    actionStop->setEnabled(true);
    tabWidget->setCurrentIndex(TAB_PLAYING);
    stationWidget->done();

    QString msg = QString(tr("%1 - %2").arg(track.artist()).arg(track.title()));
    setWindowTitle(msg);
    m_trayIcon->setToolTip(msg);

    int duration = track.duration();
    QString total = QString("%1:%2")
        .arg((duration / 1000) / 60, 2, 10, QLatin1Char('0'))
        .arg((duration / 1000) % 60, 2, 10, QLatin1Char('0'));
    totalTimeLabel->setText(total);
    timeLabel->setText(QString(tr("00:00")));
    slider->setMinimum(0);
    slider->setMaximum(duration);
    slider->setValue(0);
    metadataWidget->setMetadata(track);
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
    if (SpotFm::app()->radio()->state() != Radio::Stopped) {
        SpotFm::app()->radio()->stop();
        defaultWindow();
    }
}

void MainWindow::onPlay()
{
    if (SpotFm::app()->radio()->state() == Radio::Paused) {
        tooglePlayPauseButtons(true);
        SpotFm::app()->radio()->unpause();
   }
}

void MainWindow::onPause()
{
    if (SpotFm::app()->radio()->state() == Radio::Playing) {
        tooglePlayPauseButtons(false);
        SpotFm::app()->radio()->pause();
    }
}

void MainWindow::onSkip()
{
    actionSkip->setEnabled(false);
    SpotFm::app()->radio()->skipTrack();
}

void MainWindow::onVolumeUp()
{
    int volume = volumeSlider->value() + 5;
    if (volume > 100)
        volume = 100;
    volumeSlider->setValue(volume);
}

void MainWindow::onVolumeDown()
{
    int volume = volumeSlider->value() - 5;
    if (volume < 0)
        volume = 0;
    volumeSlider->setValue(volume);
}

void MainWindow::onMute()
{
    if (volumeSlider->value() != 0) {
        m_lastVolume = volumeSlider->value();
        volumeSlider->setValue(0);
    }
    else
        volumeSlider->setValue(m_lastVolume);
}

void MainWindow::onRadioError(const QString &msg)
{
    defaultWindow();
    QMessageBox::critical(this, tr("Error"), msg);
}

void MainWindow::tooglePlayPauseButtons(bool enabled)
{
    actionPlay->setEnabled(!enabled);
    actionPause->setEnabled(enabled);
}

void MainWindow::enableSkipButton()
{
    if (!actionSkip->isEnabled())
        actionSkip->setEnabled(true);
}
