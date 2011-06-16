#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>
#include <QList>

#include <ella/track.h>

class QGridLayout;
class QHBoxLayout;
class QToolButton;
class QPushButton;

class PlaylistControlWrapper;

class PlaylistWidget: public QWidget
{
    Q_OBJECT

    public:
        PlaylistWidget(QWidget *parent);
        ~PlaylistWidget();

        void generate();
        QList<ella::Track> playlist()
        {
            return m_tracks;
        }

    public slots:
        void addNewControl();
        void removeControl();

    signals:
        void newPlaylist();

    private slots:
        void onSearchGenerate();
        void onSimilarGenerate();

    private:
        QGridLayout *m_layout;
        QHBoxLayout *m_buttonLayout;
        QToolButton *m_addButton;

        QList<PlaylistControlWrapper *> m_controls;
        QList<ella::Track> m_tracks;
};

#endif
