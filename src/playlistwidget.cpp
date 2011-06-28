#include <ctime>

#include <ella/util.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QNetworkReply>

#include "playlistwidget.h"
#include "playlistcontrolwrapper.h"


PlaylistWidget::PlaylistWidget(QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QGridLayout(this);
    setContentsMargins(10, 10, 10, 10);
    setLayout(m_layout);
    m_layout->setColumnStretch(2, 1);
    m_layout->setMargin(0);
    m_layout->setVerticalSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSizeConstraint(QLayout::SetMinimumSize);

    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setContentsMargins( 0, 0, 0, 0 );
    m_buttonLayout->setMargin(0);
    m_buttonLayout->setSpacing(0);
    m_buttonLayout->addStretch();

    m_addButton = new QToolButton( this );
    m_addButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    m_addButton->setIcon(QPixmap::fromImage(QImage(":/icons/icons/list-add.png")));
    m_addButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_addButton->setIconSize(QSize(16, 16));
    m_addButton->setMaximumSize(QSize(32, 32));
    m_addButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_addButton->setAutoRaise(true);
    m_addButton->setContentsMargins(0, 0, 0, 0);
    connect(m_addButton, SIGNAL(clicked()), this, SLOT(addNewControl()));
    m_buttonLayout->addWidget(m_addButton);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    addNewControl();

    //Initialize random number generator
    time_t now;
    now = time(NULL);
    qsrand(now);
}

PlaylistWidget::~PlaylistWidget()
{
}

void PlaylistWidget::addNewControl()
{
    m_layout->removeItem(m_buttonLayout);
    PlaylistControlWrapper *w = new PlaylistControlWrapper(m_layout, m_controls.size(), this);
    connect(w, SIGNAL(removeControl()), this, SLOT(removeControl()));
    m_controls.append(w);
    m_layout->addItem(m_buttonLayout, m_layout->rowCount(), 0, 1, 4, Qt::AlignCenter);
}

void PlaylistWidget::removeControl()
{
    PlaylistControlWrapper* w = qobject_cast<PlaylistControlWrapper*>(sender());
    m_controls.removeAll(w);
    w->removeFromLayout();
    w->deleteLater();
    if(m_controls.size() == 0) addNewControl();
}

void PlaylistWidget::generate()
{
    ella::Track::SearchParams params;
    QList<PlaylistControlWrapper *>::Iterator iter = m_controls.begin();
    bool ok;
    QString similarArtist;
    for(; iter != m_controls.end(); ++iter) {
        PlaylistControlWrapper *control = *iter;
        QString q = control->needSimilarityQuery(&ok);
        if (!ok)
            params.append(control->toParam());
        else
            similarArtist = q;
    }

    if (similarArtist.isEmpty()) {
        QNetworkReply *reply = ella::Track::search(params, 0, 50);
        connect(reply, SIGNAL(finished()), this, SLOT(onSearchGenerate()));
    }
    else {
        QNetworkReply *reply = ella::Track::getSimilar(similarArtist, params, ella::Util::Playlist);
        connect(reply, SIGNAL(finished()), this, SLOT(onSimilarGenerate()));
    }
}

void PlaylistWidget::onSearchGenerate()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    m_tracks = ella::Track::list(reply);
    emit newPlaylist();
}

void PlaylistWidget::onSimilarGenerate()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    m_tracks = ella::Track::getSimilar(reply).values();
    emit newPlaylist();
}

