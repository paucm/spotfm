#ifndef ELLA_WS_H
#define ELLA_WS_H

#include <QNetworkReply>
#include <QString>
#include <QMap>

namespace ella {

    QNetworkAccessManager* nam();

    namespace ws {

        extern const char *Username;
        extern const char *Password;

        enum Error
        {
            NoError = 0,
            UnknownError
        };

        QNetworkReply *get(const QString &path, QMap<QString, QString> params);
        QByteArray parse(QNetworkReply *reply);
    }
}

#define ELLA_WS_HOSTNAME "ella.bmat.ws"

#endif
