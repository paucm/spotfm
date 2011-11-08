#include <QCoreApplication>
#include <QFile>
#include <QNetworkReply>

#include <ella/track.h>
#include <ella/util.h>
#include <ella/ws.h>

#include "radio.h"
#include "playlistresolver.h"
#include "spotifysession.h"
#include "openal-audio.h"


Radio::Radio(QObject *parent)
 : QObject(parent)
 , m_state(Stopped)
{
    m_playlistResolver = new PlaylistResolver(this);
    connect(m_playlistResolver, SIGNAL(trackAvailable()), this, SLOT(onTrackAvailable()));

	m_audioController = new OpenalAudio(this);
    connect(m_audioController, SIGNAL(trackStarted()), this, SLOT(onTrackStarted()));
    connect(m_audioController, SIGNAL(trackEnded(int)), this, SLOT(onTrackEnded(int)));
    connect(m_audioController, SIGNAL(trackProgress(int)), this, SIGNAL(trackProgress(int)));
    connect(m_audioController, SIGNAL(error(int, QString)), this, SLOT(onAudioControllerError(int, QString)));
	m_audioController->init();
    m_audioController->start();

    m_skipLeft = 0;
}

Radio::~Radio()
{
    m_audioController->exit();
    m_audioController->wait(500);
    delete m_audioController;
}

void Radio::playStation(const QString &name)
{
   QNetworkReply *reply = ella::Track::getSimilar(
                   name, ella::Track::SearchParams(),
                   ella::Util::Playlist);
   connect(reply, SIGNAL(finished()), this, SLOT(onGotPlaylist()));
}

void Radio::onGotPlaylist()
{
    try {
        QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
        Playlist playlist = ella::Track::getSimilar(reply).values();
        m_playlistResolver->setPlaylist(playlist);
        m_playlistResolver->start();

    }
    catch(ella::ws::ParseError &e) {
        emit error(InvalidStation, tr("This item is not available for streaming"));
    }
}

void Radio::onTrackAvailable()
{
    m_audioController->enqueue(m_playlistResolver->takeNextTrack());
    if (m_audioController->state() == Stopped)
        m_audioController->play();
    else
        m_skipLeft++;
        emit skipsLeft(m_skipLeft);
}


void Radio::stop()
{
    if (state() != Stopped) {
        m_audioController->stop();
        m_audioController->clearQueue();
        if (m_playlistResolver->isRunning())
            m_playlistResolver->stop();
        setState(Stopped);
        m_skipLeft = 0;
    }
}

void Radio::skipTrack()
{
    if (m_skipLeft != 0) {
        m_audioController->loadNext();
        m_skipLeft--;
        emit skipsLeft(m_skipLeft);
    }
}

void Radio::pause()
{
    setState(Paused);
    m_audioController->pause();
}

void Radio::unpause()
{
    setState(Playing);
    m_audioController->unpause();
}

void Radio::setVolume(int volume)
{
    m_audioController->setVolume(volume);
}

void Radio::onTrackStarted()
{
    Track track = m_audioController->currentTrack();
    setState(Playing);
    emit trackStarted(track);
}

void Radio::onTrackEnded(int at)
{
    Track track = m_audioController->currentTrack();
    setState(Stopped);
    emit trackEnded(track, at);
}

void Radio::onAudioControllerError(int code, const QString &message)
{
    if(code != InvalidTrack) {
        stop();
        emit error(code, message);
    }
}
