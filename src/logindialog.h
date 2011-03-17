#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "ui_logindialog.h"

class QPushButton;

class LoginDialog : public QDialog, private Ui::LoginDialog
{
    Q_OBJECT

    public:
        LoginDialog(QString defaultUser="", QWidget *parent=0);

        QString username() const { return userEdit->text(); }
        QString password() const { return passwordEdit->text(); }
        void setPassword(const QString &password) const { passwordEdit->setText(password); }

        bool save() { return rememberCheck->checkState() == Qt::Checked; }

    private:
        QPushButton* ok() const { return buttonBox->button(QDialogButtonBox::Ok); }

    public slots:
        void authenticate();

    private slots:
        void onLoggedError(const QString &msg);
        void onLoggedIn();
};

#endif // LOGINDIALOG_H
