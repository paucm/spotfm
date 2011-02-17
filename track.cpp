#include "track.h"
#include "spotifysession.h"

Track::Track()
 : m_sp_track(0)
{
}

Track::Track(sp_track *t)
 : m_sp_track(t)
{
  sp_track_add_ref(m_sp_track);
}

Track::Track(const Track &other)
{
  this->m_sp_track = other.m_sp_track;
  if(this->isValid())
    sp_track_add_ref(this->m_sp_track);
}

Track Track::operator=(const Track &other)
{
  this->m_sp_track = other.m_sp_track;
  if(this->isValid())
    sp_track_add_ref(this->m_sp_track);
  return *this;
}

Track::~Track()
{
  if(this->isValid())
    sp_track_release(m_sp_track);
}

QString Track::artist() const
{
  if(isValid() && sp_track_num_artists(m_sp_track)) {
    sp_artist *artist = sp_track_artist(m_sp_track, 0);
    return QString::fromUtf8(sp_artist_name(artist));
  }
  return QString();
}

QString Track::title() const
{
  if(isValid())
    return QString::fromUtf8(sp_track_name(m_sp_track));
  return QString();
}

QString Track::album() const
{
  if(isValid()) {
    sp_album *album = sp_track_album(m_sp_track);
    if (album) {
      return QString::fromUtf8(sp_album_name(album));
    }
  }
  return QString();
}

sp_image *Track::albumImage(SpotifySession *session) const
{
    if(isValid()) {
        sp_album *album = sp_track_album(m_sp_track);
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
    return sp_track_duration(m_sp_track);
  return 0;
}

QString Track::uri() const
{
    if(!isValid())
        return QString();
    char buffer[512];
    sp_link *link = sp_link_create_from_track(m_sp_track, 0);
    int size = sp_link_as_string(link, buffer, 512);
    return QString::fromLocal8Bit(buffer, size);
}

bool Track::isValid() const
{
  return m_sp_track != 0;
}

bool Track::isAvailable() const
{
  if (m_sp_track && sp_track_is_loaded(m_sp_track)) // && sp_track_is_available(m_sp_track))
    return true;
  return false;
}

