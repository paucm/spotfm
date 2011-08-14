#ifndef STATIONWIDGET
#define STATIONWIDGET

#include <QWidget>

#include "ui_stationwidget.h"


class StationWidget : public QWidget, private Ui::StationWidget
{
    Q_OBJECT

    public:
        StationWidget(QWidget *parent);
        ~StationWidget();

        void setFocus();

    public slots:
        void done();

    private slots:
        void onClicked();
        void onGotSimilar();
        void onTextChanged(const QString &text);
};

#endif

