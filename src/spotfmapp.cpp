#include <QSettings>

#include "spotfmapp.h"
#include "spotifysession.h"
#include "logindialog.h"
#include "mainwindow.h"
#include "radio.h"

SpotFmApp::SpotFmApp(int &argc, char **argv) throw(SpotFmException)
    : QApplication(argc, argv)
    , m_mainWindow(0)
{
    m_settings = new QSettings(
                QSettings::IniFormat,
				QSettings::UserScope,
				QCoreApplication::organizationName(),
				QCoreApplication::applicationName());

	//Initalizate the spotify session
    SpotifySession *spSession = new SpotifySession();

    m_logoutAndQuit = false;

    LoginDialog d(m_settings->value("Username").toString());
    if (!m_settings->value("Username").toString().isEmpty() && 
            !m_settings->value("Password").toString().isEmpty()) {
        d.setPassword(m_settings->value("Password").toString());
        d.authenticate();
    }
    if (d.exec() != QDialog::Accepted) {
        throw SpotFmException();
    }
    if (d.save() &&
        (m_settings->value("Username").toString() != d.username() ||
        m_settings->value("Password").toString() != d.password())) {
        m_settings->setValue("Username", d.username());
        m_settings->setValue("Password", d.password());
    }
    connect(spSession, SIGNAL(loggedOut()), this, SLOT(onLoggedOut()));

    m_radio = new Radio();

    m_mainWindow = new MainWindow();
    m_mainWindow->show();
}

SpotFmApp::~SpotFmApp()
{
    if (m_logoutAndQuit) {
        m_settings->remove("Password");
    }
    delete m_radio;
    delete m_mainWindow;
    delete SpotifySession::self();
    delete m_settings;
    qDebug("Good Bye!");
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

