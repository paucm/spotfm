#include <QtGlobal>
#include <QDir>
#include <QTimer>
#include <QFile>
#include <QDesktopServices>
#include <QDebug>

#include "spotfmapp.h"
#include "spotifysession.h"
#include "appkey.h"
#include "chunk.h"
#include "radio.h"

SpotifySession *SpotifySession::s_self = 0;

sp_session_callbacks SpotifySession::spotifySessionCallbacks = {
    &SpotifySession::spLoggedIn,
    &SpotifySession::spLoggedOut,
    &SpotifySession::spMetadataUpdated,
    &SpotifySession::spConnectionError,
    NULL,
    &SpotifySession::spNotifyMainThread,
    &SpotifySession::spMusicDelivery,
    &SpotifySession::spPlayTokenLost,
    &SpotifySession::spLogMessage,
    &SpotifySession::spEndOfTrack,
};

void SP_CALLCONV SpotifySession::spLoggedIn(sp_session *session, sp_error error)
{
    Q_UNUSED(session);
    if (SP_ERROR_OK != error) {
        SpotifySession::self()->signalLoggedError(QString("Failed to log in to Spotify: %1").arg(sp_error_message(error)));
        return;
    }
    SpotifySession::self()->signalLoggedIn();
}

void SP_CALLCONV SpotifySession::spLoggedOut(sp_session *session)
{
    Q_UNUSED(session);
    SpotifySession::self()->signalLoggedOut();
}

void SP_CALLCONV SpotifySession::spConnectionError(sp_session *session, sp_error error)
{
    qDebug() << "connectionError: " << sp_error_message(error);
    Q_UNUSED(session);
}

void SP_CALLCONV SpotifySession::spNotifyMainThread(sp_session *session)
{
    Q_UNUSED(session);
    SpotifySession::self()->signalNotifyMainThread();
}

void SP_CALLCONV SpotifySession::spMetadataUpdated(sp_session *session)
{
  Q_UNUSED(session);
  SpotifySession::self()->signalMetadataUpdated();
}

int SP_CALLCONV SpotifySession::spMusicDelivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames)
{
    Q_UNUSED(session);

    if (!num_frames) {
        return 0;
    }
    const int numFrames = qMin(num_frames, 8192);
    Chunk c;
    c.m_data = malloc(numFrames * sizeof(int16_t) * format->channels);
	memset(c.m_data, 0, numFrames * sizeof(int16_t) * format->channels);
    memcpy(c.m_data, frames, numFrames * sizeof(int16_t) * format->channels);
    c.m_dataFrames = numFrames;
    c.m_rate = format->sample_rate;
	c.m_channels = format->channels;
    Radio *radio = SpotFm::app()->radio();
    radio->audioController()->newChunk(c);
    return numFrames;
}

void SP_CALLCONV SpotifySession::spPlayTokenLost(sp_session *session)
{
    Q_UNUSED(session);
    SpotifySession::self()->signalPlayTokenLost();
}

void SP_CALLCONV SpotifySession::spLogMessage(sp_session *session, const char *data)
{
    Q_UNUSED(session);
    Q_UNUSED(data);
}
        
void SP_CALLCONV SpotifySession::spEndOfTrack(sp_session *session)
{
    Q_UNUSED(session);
    SpotifySession::self()->signalEndOfTrack();
}
        
SpotifySession::SpotifySession()
    : m_isLoggedIn(false)
{
    s_self = this;

    connect(this, SIGNAL(notifyMainThreadSignal()), 
			this, SLOT(onNotifyMainThread()), Qt::QueuedConnection);

    QString dataDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    #if QT_VERSION >= 0x040500
    QString cacheDir = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    #else
    QString cacheDir = dataDir + "/cache";
    cacheDir = QDir::toNativeSeparators(cacheDir);
    #endif
    m_config.api_version = SPOTIFY_API_VERSION;
    m_config.cache_location = cacheDir.toLocal8Bit().constData();
    m_config.settings_location = dataDir.toLocal8Bit().constData();
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
    #if SPOTIFY_API_VERSION < 9
    sp_session_login(m_session, username.toLatin1(), password.toLatin1());
    #else
    sp_session_login(m_session, username.toLatin1(), password.toLatin1(), false);
    #endif
}

void SpotifySession::logout()
{
    if(m_isLoggedIn) {
        sp_session_logout(m_session);
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

bool SpotifySession::load(const Track &track)
{
    if(!m_isLoggedIn) return false;

    sp_error error = sp_session_player_load(m_session, track.spotifyTrack());
    if (SP_ERROR_OK != error) {
        qDebug() << "SpotifySession: Failed to load track: " << sp_error_message(error);
        return false;
    }
    sp_session_player_play(m_session, true);
    return true;
}
 
void SpotifySession::unload()
{
    if(!m_isLoggedIn) return;

    sp_session_player_play(m_session, false);
    sp_session_player_unload(m_session);
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
    Radio *radio = SpotFm::app()->radio();
    radio->audioController()->newChunk(c);
    emit endOfTrack();
}

void SpotifySession::signalPlayTokenLost()
{
    emit playTokenLost();
}

void SpotifySession::onNotifyMainThread()
{
    int timeout;
    do {
        sp_session_process_events(m_session, &timeout);
    } while (!timeout);
    QTimer::singleShot(timeout, this, SLOT(onNotifyMainThread()));
}

