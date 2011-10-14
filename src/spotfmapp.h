#ifndef SPOTFMAPP_H
#define SPOTFMAPP_H

#include <QApplication>
#include <QVariant>

class QSettings;
class MainWindow;
class SpotFmException;
class Radio;


class SpotFmApp : public QApplication
{
    Q_OBJECT

    public:
        class SpotFmException
        {};

        SpotFmApp(int &, char **) throw (SpotFmException);
        ~SpotFmApp();

        Radio *radio() const { return m_radio; }
        QSettings *settings() { return m_settings; }

    signals:
        void event(int event, const QVariant &data = QVariant());

    public slots:
        void logout();
        void logoutAndQuit();

    private slots:
        void onLoggedOut();

    private:
        MainWindow *m_mainWindow;
        bool m_logoutAndQuit;
        Radio *m_radio;
        QSettings *m_settings;
};

namespace SpotFm
{
    inline SpotFmApp *app() { return dynamic_cast<SpotFmApp *>(qApp); }
}

#endif

