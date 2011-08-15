#include <QSettings>

#include "spotfmapp.h"
#include "spotifysession.h"
#include "logindialog.h"
#include "mainwindow.h"

SpotFmApp::SpotFmApp(int &argc, char **argv) throw(SpotFmException)
    : QApplication(argc, argv)
    , m_mainWindow(0)
{
    QSettings s(QSettings::IniFormat,
				QSettings::UserScope,
				QCoreApplication::organizationName(),
				QCoreApplication::applicationName());
    
	//Initalizate the spotify session
    SpotifySession *spSession = new SpotifySession();

    m_logoutAndQuit = false;
	
    LoginDialog d(s.value("Username").toString()); 
    if (!s.value("Username").toString().isEmpty() && !s.value("Password").toString().isEmpty()) {
        d.setPassword(s.value("Password").toString());
        d.authenticate();
    }
    if (d.exec() != QDialog::Accepted) {
        throw SpotFmException();
    }
    if (d.save() && 
        (s.value("Username").toString() != d.username() || 
        s.value("Password").toString() != d.password())) {
        s.setValue("Username", d.username());
        s.setValue("Password", d.password());
    }
    connect(spSession, SIGNAL(loggedOut()), this, SLOT(onLoggedOut()));
    m_mainWindow = new MainWindow();
    m_mainWindow->show();
}

SpotFmApp::~SpotFmApp()
{
    if (m_logoutAndQuit) {
        QSettings s(QSettings::IniFormat,
				QSettings::UserScope,
				QCoreApplication::organizationName(),
				QCoreApplication::applicationName());
        s.remove("Password");
    }
    delete m_mainWindow;
    delete SpotifySession::self();
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

