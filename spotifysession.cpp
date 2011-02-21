#include "spotifysession.h"
#include "appkey.h"
#include <QTimer>
#include <QFile>
#include "radio.h"

SpotifySession *SpotifySession::s_self = 0;

sp_session_callbacks SpotifySession::spotifySessionCallbacks = {
    &SpotifySession::loggedIn,
    &SpotifySession::loggedOut,
    &SpotifySession::metadataUpdated,
    &SpotifySession::connectionError,
    NULL,
    &SpotifySession::notifyMainThread,
    &SpotifySession::musicDelivery,
    &SpotifySession::playTokenLost,
    &SpotifySession::logMessage,
    &SpotifySession::endOfTrack,
};


void SpotifySession::loggedIn(sp_session *session, sp_error error)
{
    Q_UNUSED(session);
    if (SP_ERROR_OK != error) {
        SpotifySession::self()->signalLoggedError(QString("Failed to log in to Spotify: %1").arg(sp_error_message(error)));
        return;
    }

    SpotifySession::self()->signalLoggedIn();
}
 
void SpotifySession::loggedOut(sp_session *session)
{
    Q_UNUSED(session);
    SpotifySession::self()->signalLoggedOut();
}

void SpotifySession::connectionError(sp_session *session, sp_error error)
{
    qDebug("connectionError");
    Q_UNUSED(session);
    Q_UNUSED(error);
}

void SpotifySession::notifyMainThread(sp_session *session)
{
    Q_UNUSED(session);
    SpotifySession::self()->signalNotifyMainThread();
}

void SpotifySession::metadataUpdated(sp_session *session)
{
  Q_UNUSED(session);
  SpotifySession::self()->signalMetadataUpdated();
}
        
int SpotifySession::musicDelivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames)
{
    Q_UNUSED(session);
    
    if (!num_frames) {
        return 0;
    }
    const int numFrames = qMin(num_frames, 8192);
    QMutex &m = Radio::self()->dataMutex();
    m.lock();
    Chunk c;
    c.m_data = malloc(numFrames * sizeof(int16_t) * format->channels);
    memcpy(c.m_data, frames, numFrames * sizeof(int16_t) * format->channels);
    c.m_dataFrames = numFrames;
    c.m_rate = format->sample_rate;
    Radio::self()->newChunk(c);
    m.unlock();
    Radio::self()->pcmWaitCondition().wakeAll();
    return numFrames;
}

void SpotifySession::playTokenLost(sp_session *session)
{
    Q_UNUSED(session);
    qDebug("play token lost");
}

void SpotifySession::logMessage(sp_session *session, const char *data)
{
    Q_UNUSED(session);
    Q_UNUSED(data);
    
}
        
void SpotifySession::endOfTrack(sp_session *session)
{
    Q_UNUSED(session);
    SpotifySession::self()->signalEndOfTrack();
}
        
SpotifySession::SpotifySession()
    : m_isLoggedIn(false)
{
    s_self = this;

    connect(this, SIGNAL(notifyMainThreadSignal()), this, SLOT(onNotifyMainThread()), Qt::QueuedConnection);

    m_config.api_version = SPOTIFY_API_VERSION;
    m_config.cache_location = "/tmp/spotradio";
    m_config.settings_location = "/tmp/spotradio";
    m_config.application_key = g_appkey;
    m_config.application_key_size = g_appkey_size;
    m_config.user_agent = "last.hack";
    m_config.callbacks = &SpotifySession::spotifySessionCallbacks;

    sp_session_create(&m_config, &m_session);
}
 
SpotifySession::~SpotifySession()
{
    logout();
}

void SpotifySession::login(const QString &username, const QString &password)
{
    sp_session_login(m_session, username.toLatin1(), password.toLatin1());
}

void SpotifySession::logout()
{
    if(m_isLoggedIn) {
        sp_session_logout(m_session);
        //sp_session_release(m_session);
        m_isLoggedIn = false;
    }
}

QString SpotifySession::username() const
{
    if(!m_isLoggedIn) return QString();

    sp_user *user = sp_session_user(m_session);
    const char *username = (sp_user_is_loaded(user) ? sp_user_display_name(user) : sp_user_canonical_name(user));
    return username;
}

void SpotifySession::signalNotifyMainThread()
{
    emit notifyMainThreadSignal();
}

void SpotifySession::signalLoggedError(const QString &msg)
{
    emit loggedError(msg);
}

void SpotifySession::signalLoggedIn()
{
    m_isLoggedIn = true;
    emit loggedIn();
}

void SpotifySession::signalLoggedOut()
{
  emit loggedOut();
}

void SpotifySession::signalMetadataUpdated()
{
  emit metadataUpdated();
}

void SpotifySession::signalEndOfTrack()
{
  Chunk c;
  c.m_data = 0;
  c.m_dataFrames = -1;
  c.m_rate = -1;
  QMutex &m = Radio::self()->dataMutex();
  Radio::self()->newChunk(c);
  m.unlock();
  Radio::self()->pcmWaitCondition().wakeAll();
  emit endOfTrack();
}


void SpotifySession::onNotifyMainThread()
{
    int timeout;
    do {
        sp_session_process_events(m_session, &timeout);
    } while (!timeout);
    QTimer::singleShot(timeout, this, SLOT(onNotifyMainThread()));
}


