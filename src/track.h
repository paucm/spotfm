#ifndef TRACK_H
#define TRACK_H

#include <QString>
#include <libspotify/api.h>

#include <ella/track.h>

class SpotifySession;

class Track {

    public:
        Track();
        Track(const ella::Track &track);
        Track(const Track &other);
        Track operator=(const Track &other);
        ~Track();

        sp_track *spotifyTrack() const { return m_spTrack; }
        void setSpotifyTrack(sp_track *spTrack);

        ella::Track ellaTrack() const { return m_ellaTrack; }

        QString artist() const;
        QString title() const;
        QString album() const;
        sp_image *albumImage(SpotifySession *session) const;
        int duration() const;
        QString uri() const;

        bool isValid() const;
        bool isAvailable() const;

    private:
        ella::Track m_ellaTrack;
        sp_track *m_spTrack;
};

#endif
