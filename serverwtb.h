#ifndef SERVERWTB_H
#define SERVERWTB_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QImageReader>
#include <QSslCertificate>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QDebug>
#include <QSslError>
#include <QList>
#include <QSerialPort>
#include <QDesktopServices>
#include <QFileInfo>

class ServerWtb : public QObject
{
    Q_OBJECT

public:
    explicit ServerWtb(QObject *parent = 0);    
    ~ServerWtb();
    QNetworkReply* currentReply;
    QNetworkReply* currentReplyAC;
    QNetworkReply* currentReplyGet;
    QNetworkReply* currentReplyPdf;
    QNetworkReply* currentCookie;
    QNetworkAccessManager* nam;
    void reqSession(QString username);
    void reqLogin(QString username, QString password, QString _tok);
    void reqHome(QUrl home);
    void reqModule(QString module);
    void reqModuleGet(QString module, QString param);
    void reqGetController(QString module, QString controller, QString param);
    void reqPdfController(QString module, QString controller, QString param);
    void reqAutoComplete(QString module, QString variable, QString param = "");
    void reqSubmit(QString module, QUrlQuery params);

private:
     QString mainUrl;
     QString saveFileName(const QUrl &url);
     bool saveToDisk(const QString &filename, QIODevice *data);

signals:

private slots:
    void finishedSlot(QNetworkReply* reply);
    void ignoreSSLErrors(QNetworkReply* reply,QList<QSslError> errors);

public slots:
    void serverConnect(QString paramUrl);


};

#endif // SERVERWTB_H
