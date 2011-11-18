#include <QSettings>

#include "lastfmextension.h"
#include "lastfmservice.h"
#include "lastfmwidget.h"
#include "spotfmapp.h"
#include "radio.h"


LastFmExtension::LastFmExtension()
    : m_widget(NULL)
    , m_scrobbler(NULL)
{
}

 LastFmExtension::~LastFmExtension()
 {
 }

QWidget *LastFmExtension::settingsTab()
{
    if (!m_widget) {
        m_widget = new LastFmWidget();
    }
    return m_widget;
}

LastFmService *LastFmExtension::scrobbler()
{
    if (m_scrobbler == NULL) {
        m_scrobbler = new LastFmService(m_username, m_password, m_sessionKey, this);
        connect(m_scrobbler, SIGNAL(authenticated()), this, SLOT(onAuthenticated()));
        Radio *radio = SpotFm::app()->radio();
        connect(radio, SIGNAL(trackStarted(Track)), m_scrobbler, SLOT(nowPlaying(Track)));
        connect(radio, SIGNAL(trackEnded(int)), m_scrobbler, SLOT(scrobble(int)));
    }
    return m_scrobbler;
}

void LastFmExtension::onAuthenticated()
{
    QSettings *settings = SpotFm::app()->settings();
    settings->beginGroup(name());
    settings->setValue("session_key", m_scrobbler->sessionKey());
    settings->endGroup();
}

void LastFmExtension::loadSettings()
{
    QSettings *settings = SpotFm::app()->settings();
    settings->beginGroup(name());
    m_username = settings->value("username").toString();
    m_password = settings->value("password").toString();
    m_sessionKey = settings->value("session_key").toString();
    settings->endGroup();

    if (m_widget) {
        LastFmWidget *lfmWidget = static_cast<LastFmWidget *>(m_widget);
        lfmWidget->setUsername(m_username);
        lfmWidget->setPassword(m_password);
    }
    m_scrobbler = scrobbler();
}

void LastFmExtension::saveSettings()
{
    QSettings *settings = SpotFm::app()->settings();
    settings->beginGroup(name());
    LastFmWidget *lfmWidget = static_cast<LastFmWidget *>(m_widget);
    if (lfmWidget->username() != m_username ||
            lfmWidget->password() != m_password) {
        settings->setValue("username", lfmWidget->username());
        settings->setValue("password", lfmWidget->password());
        settings->remove("session_key");
        m_username = lfmWidget->username();
        m_password = lfmWidget->password();
        m_sessionKey.clear();
        if (m_scrobbler) {
            delete m_scrobbler;
            m_scrobbler = NULL;
            m_scrobbler = scrobbler();
        }
    }
    settings->endGroup();
}

