#include <QApplication>
#include <signal.h>

#include "mainwindow.h"
#include <iostream>
#include <unistd.h>
#include <lastfm/ws.h>



/*void print_help()
{
  std::cerr << "Usage spotify_downloader -u user -p password -a spotify_uri" << std::endl;
  exit(0);
}

void player_exit(int)
{
  Player::self()->exit();
}*/


int main(int argc, char **argv)
{
  
  QApplication app(argc, argv);
/*  if(argc != 7) {
    print_help();
  }

  int c;
  extern char *optarg;
  QString user, pass, artist;

  while ((c = getopt(argc, argv, "u:p:a:")) != -1) {
      switch(c) {
          case 'u':
            user = optarg;
            break;
          case 'p':
            pass = optarg;
            break;
          case 'a':
            artist = optarg;
            break;
      }

  }

  if(user.isEmpty() || pass.isEmpty() || artist.isEmpty()) {
      print_help();
  }*/
  lastfm::ws::ApiKey = "2e4ef608fa9812f6c98220f463a92989";
  
  //(void) signal(SIGINT, player_exit);
  //Player player(user, pass, artist);
  MainWindow *mw = new MainWindow();
  mw->show();
  return app.exec();
}
