#include "track.h"
#include "spotifysession.h"

Track::Track()
    : m_ellaTrack()
    , m_spTrack(0)
{
}

Track::Track(const ella::Track &track)
    : m_ellaTrack(track)
    , m_spTrack(0)
{
}

Track::Track(const Track &other)
{
  this->m_ellaTrack = other.m_ellaTrack;
  this->m_spTrack = other.m_spTrack;
  if(this->isValid())
    sp_track_add_ref(this->m_spTrack);
}

Track Track::operator=(const Track &other)
{
  this->m_ellaTrack = other.m_ellaTrack;
  this->m_spTrack = other.m_spTrack;
  if(this->isValid())
    sp_track_add_ref(this->m_spTrack);
  return *this;
}

Track::~Track()
{
  if(this->isValid())
    sp_track_release(m_spTrack);
}

void Track::setSpotifyTrack(sp_track *spTrack)
{
    m_spTrack = spTrack;
    sp_track_add_ref(m_spTrack);
}

QString Track::artist() const
{
  if(isValid() && sp_track_num_artists(m_spTrack)) {
    sp_artist *artist = sp_track_artist(m_spTrack, 0);
    return QString::fromUtf8(sp_artist_name(artist));
  }
  return m_ellaTrack.artistName();
}

QString Track::title() const
{
  if(isValid())
    return QString::fromUtf8(sp_track_name(m_spTrack));
  return m_ellaTrack.title();
}

QString Track::album() const
{
  if(isValid()) {
    sp_album *album = sp_track_album(m_spTrack);
    if (album) {
      return QString::fromUtf8(sp_album_name(album));
    }
  }
  return QString();
}

sp_image *Track::albumImage(SpotifySession *session) const
{
    if(isValid()) {
        sp_album *album = sp_track_album(m_spTrack);
        if (album) {
            const byte *id = sp_album_cover(album);
            return sp_image_create(session->session(), id);
        }
    }
    return NULL;
}

int Track::duration() const
{
  if(isValid())
    return sp_track_duration(m_spTrack) / 1000;
  return 0;
}

QString Track::uri() const
{
    if(!isValid())
        return QString();
    char buffer[512];
    sp_link *link = sp_link_create_from_track(m_spTrack, 0);
    int size = sp_link_as_string(link, buffer, 512);
    return QString::fromLocal8Bit(buffer, size);
}

bool Track::isValid() const
{
  return m_spTrack != 0;
}

bool Track::isAvailable() const
{
  if (m_spTrack && sp_track_is_loaded(m_spTrack)) // && sp_track_is_available(m_spTrack))
    return true;
  return false;
}

