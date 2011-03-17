#include <QNetworkAccessManager>
#include <QCoreApplication>

#include <ella/ws.h>

static QNetworkAccessManager *nam = 0;

namespace ella 
{

    namespace ws
    {
        const char *Username;
        const char *Password;
    }
}

static inline QString host()
{
    return ELLA_WS_HOSTNAME;
}

static QUrl url()
{
    QUrl url;
    url.setUserName(ella::ws::Username);
    url.setPassword(ella::ws::Password);
    url.setScheme("http");
    url.setHost(host());
    return url;
}

QNetworkReply *ella::ws::get(const QString &path, QMap<QString, QString> params)
{
    QUrl url = ::url();
    url.setPath(path);

    QString query;
    QMapIterator<QString, QString> i(params);
    while (i.hasNext()) {
        i.next();
        QByteArray const key = QUrl::toPercentEncoding(i.key());
        QString v = i.value();
        QByteArray const value =  QUrl::toPercentEncoding(v.replace( ' ', '+' ), "+");
        url.addEncodedQueryItem( key, value );
    }
    qDebug("%s", url.toString(QUrl::RemoveUserInfo).toLocal8Bit().constData());
    return ella::nam()->get(QNetworkRequest(url));
}

QByteArray ella::ws::parse(QNetworkReply *reply)
{
    QByteArray array = reply->readAll();
    if(reply->error() != QNetworkReply::NoError) {
        //TODO check errors
        qDebug("error");
    }
    reply->deleteLater();
    return array; 
}

QNetworkAccessManager* ella::nam()
{
    if (!::nam) ::nam = new QNetworkAccessManager( qApp );
    return ::nam;
}
