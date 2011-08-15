#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QSystemTrayIcon>

#include "ui_mainwindow.h"

class Radio;
class Track;
class QCloseEvent;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *widget=0, Qt::WFlags fl = 0);
        ~MainWindow();

    private:
        void toggleWindowVisibility();

    private slots:
        void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
        void restoreWindow();

        void onPlay();
        void onStop();
        void onPause();
        void onSkip();
        void onVolumeUp();
        void onVolumeDown();
        void onMute();

        void onPlaying(const Track &track);
        void onTrackProgress(int pos);
        void onRadioError(const QString &msg);
        void enableSkipButton();

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        void setupTrayIcon();
        void defaultWindow();
        void createStation();
        void tooglePlayPauseButtons(bool enabled);

        Radio *m_radio;
        QSystemTrayIcon *m_trayIcon;
        int m_lastVolume;
};

#endif
