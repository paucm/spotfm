#include <ella/ws.h>

#include "spotfmapp.h"
#include "mainwindow.h"

int main(int argc, char **argv)
{
    QCoreApplication::setOrganizationName("SpotFm");
    QCoreApplication::setApplicationName("SpotFm");
    QCoreApplication::setApplicationVersion("0.1");
    try
    {
        ella::ws::Username = "";
        ella::ws::Password = "";
        SpotFmApp app(argc, argv);
        return app.exec();
    }
    catch(SpotFmApp::SpotFmException)
    {
        return 0;
    }
}
