#ifndef STATION_H
#define STATION_H

/***************
Examples 
lastfm://globaltag/rock
lastfm://artist/Queen/similarartists
lastfm://user/sebr/personal
****************/

class Station {
  public:
    Station(const QString &url);
    ~Station();
    
    void parse(QString &url);
};

class StationWS {
  public:
    StationWs(const QString &query); 
}


#endif