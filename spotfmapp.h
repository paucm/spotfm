#ifndef SPOTFMAPP_H
#define SPOTFMAPP_H

#include <QApplication>

class SpotFmException;

class SpotFmApp : public QApplication
{
    public:
        class SpotFmException
        {};

        SpotFmApp(int &, char **) throw (SpotFmException);
        ~SpotFmApp();
};



#endif
