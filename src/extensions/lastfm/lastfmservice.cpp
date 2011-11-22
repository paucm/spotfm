#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QUrl>
#include <QDateTime>
#include <QTimer>
#include <QDebug>

#include <ella/xmlquery.h>
#include "lastfmservice.h"


QString md5(const QByteArray &src)
{
    QByteArray const digest = QCryptographicHash::hash(src, QCryptographicHash::Md5);
    return QString::fromLatin1(digest.toHex()).rightJustified(32, '0');
}

static QUrl url()
{
    QUrl url;
    url.setScheme("http");
    url.setHost("ws.audioscrobbler.com");
    url.setPath("/2.0");
    return url;
}


QString LastFmService::apiKey = "2e4ef608fa9812f6c98220f463a92989";
QString LastFmService::secret = "08d08b54530bf8855dd9013756863514";


LastFmService::LastFmService(const QString &username, const QString &password, const QString &sessionKey, QObject *parent)
    : QObject(parent)
    , m_username(username)
    , m_password(password)
    , m_sessionKey(sessionKey)
    , m_timestamp(0)
{

    m_nam = new QNetworkAccessManager(this);
    if (!username.isEmpty() && !password.isEmpty())
        authenticate();

    m_timer = new QTimer( this );
    m_timer->setSingleShot(true);
    connect( m_timer, SIGNAL(timeout()), SLOT(requestNowPlaying()));
}

LastFmService::~LastFmService()
{
    m_timer->stop();
}

void LastFmService::authenticate()
{
    const QString authToken = md5(QString("%1%2").
                                  arg(m_username).
                                  arg(md5(m_password.toUtf8())).toUtf8());
    const QString apiSig = md5(QString("api_key%1authToken%2methodauth.getMobileSessionusername%3%4").
                               arg(LastFmService::apiKey).
                               arg(authToken).
                               arg(m_username).
                               arg(LastFmService::secret).toUtf8());
    // now authenticate to last.fm and get our session key
    if (m_sessionKey.isEmpty()) {
        qDebug() << "LASTFM: Got not saved session key, authenticating with last.fm";
        QUrl url = ::url();
        url.addEncodedQueryItem("username", m_username.toUtf8());
        url.addEncodedQueryItem("authToken", authToken.toUtf8());
        url.addEncodedQueryItem("api_key", LastFmService::apiKey.toUtf8());
        url.addEncodedQueryItem("api_sig", apiSig.toUtf8());
        url.addEncodedQueryItem("method", "auth.getMobileSession");
        QNetworkReply *reply = m_nam->get(QNetworkRequest(url));
        connect(reply, SIGNAL(finished()), this, SLOT(onAuthenticated()));
    }
    else {
        qDebug() << "LASTFM: Using save session key from last.fm";
    }
}


void LastFmService::onAuthenticated()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());

    switch(reply->error()) {
        case QNetworkReply::NoError:
        {
            ella::XmlQuery xml(reply->readAll());
            if (xml.children("error").size() > 0) {
                qDebug() << "LASTFM: Error from authenticating with last.fm service";
            }
            m_sessionKey = xml["session"]["key"].text();
            emit authenticated();
            break;
        }
        default:
            qDebug() << "LASTFM: There was a problem communicating with the Last.fm services";
    }
    reply->deleteLater();
}

void LastFmService::nowPlaying(const Track &track)
{
    if (m_sessionKey.isEmpty()) return;

    m_currentTrack = track;
    m_timestamp = QDateTime::currentDateTime().toUTC().toTime_t();
    //Wait 5 seconds before submit nowPlaying
    m_timer->start(5*1000);
}

void LastFmService::requestNowPlaying()
{
    if (m_sessionKey.isEmpty()) return;

    const QString apiSig = md5(QString(
                                   "album%1api_key%2artist%3duration%4methodtrack.updateNowPlayingsk%5track%6%7").
                               arg(m_currentTrack.album()).
                               arg(LastFmService::apiKey).
                               arg(m_currentTrack.artist()).
                               arg(m_currentTrack.duration()).
                               arg(m_sessionKey).
                               arg(m_currentTrack.title()).
                               arg(LastFmService::secret).toUtf8());
    #define e( x ) QUrl::toPercentEncoding(x)
    QByteArray data = "track=" + e(m_currentTrack.title()) +
                      "&artist=" + e(m_currentTrack.artist()) +
                      "&album=" + e(m_currentTrack.album()) +
                      "&duration=" + QByteArray::number(m_currentTrack.duration()) +
                      "&api_key=" + e(LastFmService::apiKey) +
                      "&api_sig=" + e(apiSig) +
                      "&sk=" + e(m_sessionKey) +
                      "&method=track.updateNowPlaying";
    #undef e
    qDebug() << "LASTFM NowPlaying:" << data;
    QNetworkReply *reply = m_nam->post(QNetworkRequest(url()), data);
    connect(reply, SIGNAL(finished()), this, SLOT(onNowPlaying()));
}

void LastFmService::onNowPlaying()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();
}

void LastFmService::scrobble(int at)
{
    if (m_sessionKey.isEmpty()) return;

    m_timer->stop();

    int point = (at * 100) / m_currentTrack.duration();
    if (m_currentTrack.duration() < scrobbleMinDuration ||
            !(at > scrobbleMaxDuration || point > defaultScrobblePoint))
        return;

    const QString apiSig = md5(QString(
                                   "album%1api_key%2artist%3chosenByUser0duration%4methodtrack.scrobblesk%5timestamp%6track%7%8").
                               arg(m_currentTrack.album()).
                               arg(LastFmService::apiKey).
                               arg(m_currentTrack.artist()).
                               arg(m_currentTrack.duration()).
                               arg(m_sessionKey).
                               arg(m_timestamp).
                               arg(m_currentTrack.title()).
                               arg(LastFmService::secret).toUtf8());

    #define e( x ) QUrl::toPercentEncoding(x)
    QByteArray data = "timestamp=" + QByteArray::number(m_timestamp) +
                      "&track=" + e(m_currentTrack.title()) +
                      "&artist=" + e(m_currentTrack.artist()) +
                      "&album=" + e(m_currentTrack.album()) +
                      "&duration=" + QByteArray::number(m_currentTrack.duration()) +
                      "&api_key=" + e(LastFmService::apiKey) +
                      "&api_sig=" + e(apiSig) +
                      "&sk=" + e(m_sessionKey) +
                      "&chosenByUser=0" +
                      "&method=track.scrobble";
    #undef e
    qDebug() << "LASTM Scrobble:" << data;
    QNetworkReply *reply = m_nam->post(QNetworkRequest(url()), data);
    connect(reply, SIGNAL(finished()), this, SLOT(onScrobble()));
}

void LastFmService::onScrobble()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();
}

