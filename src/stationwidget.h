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
        QString name() const;

    signals:
        void clicked();

    public slots:
        void startProgress();
        void stopProgress();

    private slots:
        void onTextChanged(const QString &text);
};

#endif

