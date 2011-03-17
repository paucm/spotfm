#ifndef SPOTFMAPP_H
#define SPOTFMAPP_H

#include <QApplication>

class SpotFmException;

class SpotFmApp : public QApplication
{
    Q_OBJECT

    public:
        class SpotFmException
        {};

        SpotFmApp(int &, char **) throw (SpotFmException);
        ~SpotFmApp();

    public slots:
        void logout();
        void logoutAndQuit();
    
    private slots:
        void onLoggedOut();

    private:
        bool m_logoutAndQuit;
};



#endif
