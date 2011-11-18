#include <QTabWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "settingsdialog.h"
#include "extension.h"


SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{

    m_tab = new QTabWidget(this);
    QPushButton *closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, SIGNAL(released()), this, SLOT(onClose()));

    QVBoxLayout *vl = new QVBoxLayout;
    vl->addWidget(m_tab);
    QHBoxLayout *hl = new QHBoxLayout;
    hl->addStretch();
    hl->addWidget(closeButton);
    vl->addLayout(hl);
    setLayout(vl);
    setModal(true);

    setWindowTitle(tr("Settings"));
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::addExtension(Extension *ext)
{
    m_extensions.append(ext);
    if (ext->hasSettingsTab()) {
        QWidget *wdg = ext->settingsTab();
        wdg->setParent(m_tab);
        m_tab->addTab(wdg, ext->name());
        ext->loadSettings();
    }
}

void SettingsDialog::onClose()
{
    for (int i = 0; i < m_extensions.size(); ++i) {
        m_extensions.at(i)->saveSettings();
    }
    hide();
}

