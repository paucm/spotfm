#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QList>

class QTabWidget;
class Extension;

class SettingsDialog : public QDialog
{
    Q_OBJECT

    public:
        SettingsDialog(QWidget *parent=0);
        ~SettingsDialog();

        void addExtension(Extension *ext);

    private slots:
        void onClose();

    private:
        QTabWidget *m_tab;
        QList<Extension *> m_extensions;
};


#endif

