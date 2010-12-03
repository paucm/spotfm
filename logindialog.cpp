#include "logindialog.h"

LoginDialog::LoginDialog( QWidget* parent,
                          QString defaultUser )
        : QDialog(parent)
        , Ui::LoginDialog()
{
    setupUi( this );
    if(!defaultUser.isEmpty())
        userEdit->setText(defaultUser);
}

