#include "spotifysession.h"
#include "stationwidget.h"


StationWidget::StationWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    helloLabel->setText(QString(tr("Hello %1,")).arg(SpotifySession::self()->username()));
    connect(edit, SIGNAL(returnPressed()), this, SIGNAL(clicked()));
    connect(edit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(playButton, SIGNAL(clicked()), this, SIGNAL(clicked()));
    playButton->setEnabled(false);
    progressLabel->hide();
}


StationWidget::~StationWidget()
{
}

void StationWidget::setFocus()
{
    edit->setFocus();
}

QString StationWidget::name() const
{
    return edit->text();
}

void StationWidget::onTextChanged(const QString &text)
{
    playButton->setEnabled(!text.isEmpty());
}

void StationWidget::stopProgress()
{
    playButton->setEnabled(true);
    progressLabel->hide();
}

void StationWidget::startProgress()
{
    edit->onCompleted();
    playButton->setEnabled(false);
    progressLabel->show();
}

