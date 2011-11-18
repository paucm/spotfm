#include "lastfmwidget.h"


LastFmWidget::LastFmWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}

LastFmWidget::~LastFmWidget()
{
}

void LastFmWidget::setUsername(const QString &text)
{
    usernameEdit->setText(text);
}

void LastFmWidget::setPassword(const QString &text)
{
    passwordEdit->setText(text);
}


QString LastFmWidget::username() const
{
    return usernameEdit->text();
}

QString LastFmWidget::password() const
{
    return passwordEdit->text();
}

