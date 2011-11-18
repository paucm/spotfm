#ifndef LASTFMEXTENSION_H
#define LASTFMEXTENSION_H

#include <QString>

#include "extension.h"
#include "track.h"


class LastFmService;


class LastFmExtension : public Extension
{
    Q_OBJECT

    public:
        LastFmExtension();
        virtual ~LastFmExtension();

        virtual QString name() const { return "LastFm"; }
        virtual bool hasSettingsTab() const  { return true; }
        virtual QWidget *settingsTab();

        virtual void loadSettings();
        virtual void saveSettings();

    private slots:
        void onAuthenticated();

    private:
        LastFmService *scrobbler();

        QWidget *m_widget;
        LastFmService *m_scrobbler;

        QString m_username;
        QString m_password;
        QString m_sessionKey;
};


#endif

