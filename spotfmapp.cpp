#include <QSettings>

#include "spotfmapp.h"
#include "spotifysession.h"
#include "logindialog.h"

SpotFmApp::SpotFmApp(int &argc, char **argv) throw(SpotFmException)
    : QApplication(argc, argv)
{
    QSettings s;
    //Initalizate the spotify session
    SpotifySession *spSession = new SpotifySession();

    m_logoutAndQuit = false;

    if (s.value("Username").toString().isEmpty() || s.value("Password").toString().isEmpty()) {
        LoginDialog d(s.value("Username").toString());
        if (d.exec() != QDialog::Accepted) {
            throw SpotFmException();
        }
        if (d.save()) {
            s.setValue("Username", d.username());
            s.setValue("Password", d.password());
        }
    }
    else {
        spSession->login(s.value("Username").toString(), s.value("Password").toString());
    }
    connect(spSession, SIGNAL(loggedOut()), this, SLOT(onLoggedOut()));
}

SpotFmApp::~SpotFmApp()
{
    if (m_logoutAndQuit) {
        QSettings s;
        s.remove("Password");
    }
    delete SpotifySession::self();
}

void SpotFmApp::logout()
{
    SpotifySession::self()->logout();
}

void SpotFmApp::logoutAndQuit()
{
    m_logoutAndQuit = true;
    logout();
}

void SpotFmApp::onLoggedOut()
{
    quit();
}

