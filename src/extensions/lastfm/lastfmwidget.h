#ifndef LASTFMWIDGET_H
#define LASTFMWIDGET_H

#include <QWidget>
#include <ui_lastfmwidget.h>


class LastFmWidget : public QWidget, private Ui::LastFmWidget
{
    Q_OBJECT

    public:
        LastFmWidget(QWidget *parent=0);
        ~LastFmWidget();

        void setUsername(const QString &text);
        void setPassword(const QString &text);

        QString username() const;
        QString password() const;
};

#endif

