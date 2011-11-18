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
#include "settingsdialog.h"
#include "metadatawidget.h"
#include "util.h"

#include "extensions/lastfm/lastfmextension.h"

#define TAB_STATION 0
#define TAB_PLAYING 1


MainWindow::MainWindow(QWidget *widget, Qt::WFlags fl)
  : QMainWindow(widget, fl)
  , Ui::MainWindow()
{
    setupUi(this);

    Ui_MainWindow::mainToolBar->setIconSize(QSize(35, 35));

    connect(actionVolumeUp, SIGNAL(triggered()), this, SLOT(onVolumeUp()));
    connect(actionVolumeDown, SIGNAL(triggered()), this, SLOT(onVolumeDown()));
    connect(actionMute, SIGNAL(triggered()), this, SLOT(onMute()));
    connect(actionSkip, SIGNAL(triggered()), this, SLOT(onSkip()));
    connect(actionStop, SIGNAL(triggered()), this, SLOT(onStop()));
    connect(actionPlay, SIGNAL(triggered()), this, SLOT(onUnpause()));
    connect(actionPause, SIGNAL(triggered()), this, SLOT(onPause()));

    actionPlay->setVisible(false);

    connect(actionLogoutAndQuit, SIGNAL(triggered()), qApp, SLOT(logoutAndQuit()));
    connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(logout()));

    AboutDialog *about = new AboutDialog(this);
    connect(actionAbout, SIGNAL(triggered()), about, SLOT(show()));

    m_settingsDlg = new SettingsDialog(this);
    connect(actionSettings, SIGNAL(triggered()), m_settingsDlg, SLOT(show()));

    Radio *radio = SpotFm::app()->radio();
    connect(radio, SIGNAL(trackStarted(Track)), this, SLOT(onTrackStarted(Track)));
    connect(radio, SIGNAL(skipsLeft(int)), this, SLOT(enableSkipButton(int)));
    connect(radio, SIGNAL(trackProgress(int)), this, SLOT(onTrackProgress(int)));
    connect(radio, SIGNAL(error(int, QString)), this, SLOT(onRadioError(int, QString)));

    connect(volumeSlider, SIGNAL(valueChanged(int)), radio, SLOT(setVolume(int)));

    connect(stationWidget, SIGNAL(clicked()), this, SLOT(onNewStation()));
    stationWidget->setFocus();

    QSettings *s = SpotFm::app()->settings();
    volumeSlider->setValue(s->value("volume", 80).toInt());
    radio->setVolume(volumeSlider->value());
    m_lastVolume = volumeSlider->value();

    defaultWindow();
    setupTrayIcon();
    loadExtensions();
}

MainWindow::~MainWindow()
{
    QSettings *s = SpotFm::app()->settings();
    s->setValue("volume", volumeSlider->value());
}

void MainWindow::defaultWindow()
{
    setWindowTitle("SpotFm");
    actionStop->setEnabled(false);
    actionSkip->setEnabled(false);
    actionPlay->setEnabled(false);
    actionPause->setEnabled(false);
    timeLabel->setText(QString(tr("--:--")));
    totalTimeLabel->setText(QString(tr("--:--")));
    slider->setValue(0);
    tabWidget->setCurrentIndex(TAB_STATION);
    metadataWidget->clear();
    Ui_MainWindow::statusBar->clearMessage();
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

void MainWindow::onNewStation()
{
    stationWidget->startProgress();
    Radio *radio = SpotFm::app()->radio();
    if (radio->state() != Stopped)
        radio->stop();
    radio->playStation(stationWidget->name());
    Ui_MainWindow::statusBar->showMessage(
            QString(tr("Playing tracks %1")).arg(radio->stationName()));
}

void MainWindow::onTrackStarted(const Track &track)
{
    actionStop->setEnabled(true);
    actionSkip->setEnabled(true);
    actionPause->setEnabled(true);
    actionPlay->setEnabled(true);
    tabWidget->setCurrentIndex(TAB_PLAYING);
    stationWidget->stopProgress();

    QString msg = QString(tr("%1 - %2").arg(track.artist()).arg(track.title()));
    setWindowTitle(msg);
    m_trayIcon->setToolTip(msg);

    int duration = track.duration();
    QString total = QString("%1:%2")
        .arg(duration / 60, 2, 10, QLatin1Char('0'))
        .arg(duration % 60, 2, 10, QLatin1Char('0'));
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
        .arg(slider->value() / 60, 2, 10, QLatin1Char('0'))
        .arg(slider->value() % 60, 2, 10, QLatin1Char('0'));
    timeLabel->setText(progress);
}

void MainWindow::onStop()
{
    SpotFm::app()->radio()->stop();
    defaultWindow();
}

void MainWindow::onSkip()
{
    SpotFm::app()->radio()->skipTrack();
}

void MainWindow::onPause()
{
    actionPlay->setVisible(true);
    actionPlay->setEnabled(true);
    actionPause->setVisible(false);
    SpotFm::app()->radio()->pause();
}

void MainWindow::onUnpause()
{
    actionPlay->setVisible(false);
    actionPlay->setEnabled(false);
    actionPause->setVisible(true);
    actionPause->setEnabled(true);
    SpotFm::app()->radio()->unpause();
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

void MainWindow::onRadioError(int code, const QString &msg)
{
    switch(code) {
        case InvalidStation:
            stationWidget->stopProgress();
            break;
        default:
            defaultWindow();
    };
    QMessageBox::critical(this, tr("Error"), msg);
}

void MainWindow::enableSkipButton(int skips)
{
    actionSkip->setEnabled(skips > 0);
}

void MainWindow::loadExtensions()
{
    LastFmExtension *ext = new LastFmExtension();
    ext->setParent(this);
    m_settingsDlg->addExtension(ext);
}

