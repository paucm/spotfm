#ifndef SPOTIFYSESSION_H
#define SPOTIFYSESSION_H

#include <QObject>
#include <libspotify/api.h>

class QFile;
class AudioEngine;

class SpotifySession : public QObject
{
    Q_OBJECT

    public:
        SpotifySession();
        ~SpotifySession();


        void login(const QString &username, const QString &password);
        void logout();

        sp_session *session() { return m_session; }
        QString username() const;
        bool isLoggedIn() const { return m_isLoggedIn; }

        static SpotifySession *self() { return s_self; }

    signals:
        void notifyMainThreadSignal();
        void loggedIn();
        void loggedOut();
        void loggedError(const QString msg);
        void metadataUpdated();
        void endOfTrack();
        void playTokenLost();

    private slots:
        void onNotifyMainThread();

    private:
        void signalNotifyMainThread();
        void signalLoggedError(const QString &msg);
        void signalLoggedIn();
        void signalLoggedOut();
        void signalMetadataUpdated();
        void signalEndOfTrack();
        void signalPlayTokenLost();

        static void loggedIn(sp_session *session, sp_error error);
        static void connectionError(sp_session *session, sp_error error);
        static void loggedOut(sp_session *session);
        static void notifyMainThread(sp_session *session);
        static void logMessage(sp_session *session, const char *data);
        static int musicDelivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames);
        static void endOfTrack(sp_session *session);
        static void playTokenLost(sp_session *session);
        static void metadataUpdated(sp_session *session);
        
        static sp_session_callbacks spotifySessionCallbacks;

        static SpotifySession *s_self;

        sp_session *m_session;
        sp_session_config m_config;

        bool m_isLoggedIn;
};

#endif

