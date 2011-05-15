#ifndef PLAYLISTCONTROLWRAPPER_H
#define PLAYLISTCONTROLWRAPPER_H

#include <QObject>
#include <QWidget>

#include <ella/track.h>

class QComboBox;
class QToolButton;
class PlaylistControl;
class QGridLayout;

class PlaylistControlWrapper : public QObject
{
    Q_OBJECT

    public:
        PlaylistControlWrapper(QGridLayout *layout,
                              int row,
                              QWidget *parent=0);
        ~PlaylistControlWrapper();

        void removeFromLayout();

        ella::Track::SearchParamEntry toParam() const;

    signals:
        void removeControl();

    public slots:
        void selectorChanged(const QString &type);

    private:
        void showControl();
        
    private:
        PlaylistControl *m_control;
        QComboBox *m_selector;
        QToolButton *m_minusButton;
        QGridLayout *m_layout;
        int m_row;
};

#endif

