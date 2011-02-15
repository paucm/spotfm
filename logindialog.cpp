#include <QMessageBox>
#include <QPushButton>

#include "logindialog.h"
#include "spotifysession.h"

LoginDialog::LoginDialog( QString defaultUser,
                          QWidget *parent)
        : QDialog(parent)
        , Ui::LoginDialog()
{
    setupUi( this );
    if(!defaultUser.isEmpty())
        userEdit->setText(defaultUser);

    connect(Ui::LoginDialog::buttonBox, SIGNAL(accepted()), SLOT(authenticate()));
}

void LoginDialog::authenticate()
{
    ok()->setEnabled(false);
    setWindowTitle(QString(tr("Verifying Login...")));
    SpotifySession *spSession = SpotifySession::self();
    connect(spSession, SIGNAL(loggedError(QString)), this, SLOT(onLoggedError(QString)));
    connect(spSession, SIGNAL(loggedIn()), this, SLOT(onLoggedIn()));
    spSession->login(username(), password());
}

void LoginDialog::onLoggedError(const QString &msg)
{
    QMessageBox::critical(this, tr("Login Error"), msg);
    userEdit->setText(username());
    passwordEdit->setText(QString());
    ok()->setEnabled(true);
    retranslateUi( this ); //reset window title
}

void LoginDialog::onLoggedIn()
{
    accept();
}
