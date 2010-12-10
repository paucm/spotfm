#include <QApplication>

#include <lastfm/ws.h>

#include "mainwindow.h"


int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  
  lastfm::ws::ApiKey = "";
  
  MainWindow *mw = new MainWindow();
  mw->show();
  return app.exec();
}
