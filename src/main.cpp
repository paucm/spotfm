#include <ella/ws.h>

#include "spotfmapp.h"
#include "mainwindow.h"

int main(int argc, char **argv)
{
    QCoreApplication::setOrganizationName("pcapella");
    QCoreApplication::setApplicationName("SpotFm");
    try
    {
        SpotFmApp app(argc, argv);
        ella::ws::Username = "";
        ella::ws::Password = "";
        MainWindow *mw = new MainWindow();
        mw->show();
        return app.exec();
    }
    catch(SpotFmApp::SpotFmException)
    {
        return 0;
    }
}
