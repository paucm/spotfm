#ifndef TRACK_H
#define TRACK_H

#include <QString>
#include <libspotify/api.h>

class SpotifySession;

class Track {
  public:
    Track();
    Track(sp_track *t);
    Track(const Track &other);
    Track operator=(const Track &other);
    ~Track();

    sp_track *spotifyTrack() const { return m_sp_track; }
    QString artist() const;
    QString title() const;
    QString album() const;
    sp_image *albumImage(SpotifySession *session) const;
    int duration() const;

    bool isValid() const;
    bool isAvailable() const;

  private:
    sp_track *m_sp_track;
};

#endif
