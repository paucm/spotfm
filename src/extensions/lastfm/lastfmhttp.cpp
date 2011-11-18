#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>

#include <ella/xmlquery.h>
#include "lastfmhttp.h"

LastFmHttp::LastFmHttp(QObject *parent) :
    QObject(parent)
{
    m_nam = new QNetworkAccessManager(this);
}

LastFmHttp::~LastFmHttp()
{
    delete m_nam;
}

void LastFmHttp::request(const QUrl &url)
{
    QNetworkReply *reply = m_nam->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
}

void LastFmHttp::onFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        emit done(reply->readAll());
    }
    else {        
        emit done(QByteArray());
        qDebug() << "There was a problem communicating with the Last.fm service";
    }
    reply->deleteLater();
}
