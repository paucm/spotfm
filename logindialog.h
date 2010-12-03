#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "ui_logindialog.h"

class LoginDialog : public QDialog, private Ui::LoginDialog
{
    Q_OBJECT

    public:
        LoginDialog( QWidget* parent, QString  defaultUser = "" );
        
        QString username() const { return userEdit->text(); }
        QString password() const { return passwordEdit->text(); }
        bool save() { return rememberCheck->checkState() == Qt::Checked; }
};

#endif // LOGINDIALOG_H
