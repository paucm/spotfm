#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "ui_mainwindow.h"
#include "chunk.h"

class Radio;
class Track;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *widget=0, Qt::WFlags fl = 0);
        ~MainWindow();

    private slots:
        void onPlay();
        void onStop();
        void onPause();
        void onSkip();

        void onPlaying(const Track &track);
        void onTrackProgress(const Chunk &chunk);
        void onArtistImage(QImage);
        void onRadioError(const QString &msg);
        void enableSkipButton();

    private:
        void defaultWindow();
        QString customStation();
        void toogleButtons(bool enabled);

        Radio *m_radio;
};

#endif
