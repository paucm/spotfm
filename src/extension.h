#ifndef EXTENSION_H
#define EXTENSION_H

#include <QWidget>


class Extension : public QObject
{
    Q_OBJECT

    public:
        virtual ~Extension() {}

        virtual QString name() const = 0;

        virtual bool hasSettingsTab() const { return false; }
        virtual QWidget *settingsTab() { return 0; }
        virtual QString settingsCaption() { return ""; }

        virtual void loadSettings() {}
        virtual void saveSettings() {}
};


#endif

