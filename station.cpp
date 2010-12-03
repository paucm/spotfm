#include "station.h"

Station::Station(const QString &url)
{
}

Station::~Station()
{
}

void Station::parse(const QString &url)
{
  if(url.isEmpty()) return QString();
  
  QStringList elements = QStringList::split("/", url);
  
  //ArtistRadios
  //lastfm://artist/Queen/similarartists
  if(elements[1] == "artist") {
    std::cout << elements[2].toLocal8Bit() << std::endl;
  }
  
  else if(elements[1] == "user") {
    if ( elements[3] == "personal" ) {
      std::cout << elements[2].toLocal8Bit() << std::endl;
    }
  }
  
}