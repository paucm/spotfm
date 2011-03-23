#ifndef ARTISTSTATION_H
#define ARTISTSTATION_H

#include "station.h"

class ArtistStation : public QueryStation
{
    Q_OBJECT
    public:
        ArtistStation(const QString &name, QObject *parent = 0);
        ~ArtistStation();

    protected:
        void search();

    private slots:
        void onGotSearch();
        void onGotSimilar();
};

#endif
