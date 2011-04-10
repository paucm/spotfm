#ifndef TRACKSTATION_H
#define TRACKTSTATION_H

#include "station.h"

class TrackStation : public QueryStation
{
    Q_OBJECT
    public:
        TrackStation(const QString &name, QObject *parent = 0);
        ~TrackStation();

    protected:
        void search();

    private slots:
        void onGotSearch();
        void onGotSimilar();
};

#endif
