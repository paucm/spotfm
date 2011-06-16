#include <QComboBox>
#include <QToolButton>
#include <QGridLayout>

#include "playlistcontrolwrapper.h"
#include "playlistcontrol.h"

PlaylistControlWrapper::PlaylistControlWrapper(
                            QGridLayout *layout,
                            int row,
                            QWidget *parent)
    : m_control(PlaylistControl::get())
    , m_layout(layout)
    , m_row(row)
{
    m_selector = new QComboBox(parent);
    m_selector->addItems(PlaylistControl::controls());
    m_layout->addWidget(m_selector, row, 0, Qt::AlignLeft);
    connect(m_selector, SIGNAL(activated(QString)), this, SLOT(selectorChanged(QString)));


    m_minusButton = new QToolButton(parent);
    m_minusButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);   
    m_minusButton->setIcon(QPixmap::fromImage(QImage(":/icons/icons/list-remove.png")));
    m_minusButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_minusButton->setIconSize(QSize(16, 16));
    m_minusButton->setMaximumSize(QSize(32, 32));
    m_minusButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_minusButton->setAutoRaise(true);
    m_minusButton->setContentsMargins(0, 0, 0, 0);
    connect(m_minusButton, SIGNAL(clicked(bool)), this, SIGNAL(removeControl()));

    showControl();
    m_layout->addWidget(m_minusButton, m_row, 3, Qt::AlignCenter);
}

PlaylistControlWrapper::~PlaylistControlWrapper()
{
    removeFromLayout();
    delete m_control;
    delete m_selector;
    delete m_minusButton;
}

void PlaylistControlWrapper::selectorChanged(const QString &type)
{
    m_layout->removeWidget(m_control->match());
    m_layout->removeWidget(m_control->input());
    delete m_control;
    m_control = PlaylistControl::get(type);
    showControl();
}

void PlaylistControlWrapper::showControl()
{
    if (m_control->match()) {
        m_layout->addWidget(m_control->match(), m_row, 1, Qt::AlignCenter);
        m_control->match()->show();
    }

    if (m_control->input()) {
        m_layout->addWidget(m_control->input(), m_row, 2);
        m_control->input()->show();
    }   
}

void PlaylistControlWrapper::removeFromLayout()
{
    if (m_control->match()) {
        m_layout->removeWidget(m_control->match());
    }

    if (m_control->input()) {
        m_layout->removeWidget(m_control->input());
    }
    m_layout->removeWidget(m_selector);
    m_layout->removeWidget(m_minusButton);
}

ella::Track::SearchParamEntry PlaylistControlWrapper::toParam() const {
    return m_control->toParam();
}

QString PlaylistControlWrapper::needSimilarityQuery(bool *ok) const
{
    return m_control->needSimilarityQuery(ok);
}
