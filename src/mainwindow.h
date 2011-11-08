#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QSystemTrayIcon>

#include "ui_mainwindow.h"

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

        void onNewStation();

        void onStop();
        void onSkip();
        void onPause();
        void onUnpause();
        void onVolumeUp();
        void onVolumeDown();
        void onMute();
        void enableSkipButton(int skips);

        void onTrackStarted(const Track &track);
        void onTrackProgress(int pos);
        void onRadioError(int code, const QString &msg);

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        void setupTrayIcon();
        void defaultWindow();
        void createStation();
        void tooglePlayPauseButtons(bool enabled);

        QSystemTrayIcon *m_trayIcon;
        int m_lastVolume;
};

#endif

