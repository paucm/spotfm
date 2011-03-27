#ifndef TAGSTATION_H
#define TAGSTATION_H

#include "station.h"

class TagStation : public QueryStation
{
    Q_OBJECT
    public:
        TagStation(const QString &name, QObject *parent = 0);
        ~TagStation();

    protected:
        void search();

    private slots:
        void onGotSearch();
};

#endif
