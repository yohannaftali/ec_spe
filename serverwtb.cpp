#include "serverwtb.h"

ServerWtb::ServerWtb(QObject *parent) :
    QObject(parent)
{
    qDebug() << "construct connection to server";
    // Create QNetworkAccessManager and start listening for its finished signal.
    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));
    QObject::connect(nam, SIGNAL(sslErrors(QNetworkReply*, QList<QSslError>)), this, SLOT(ignoreSSLErrors(QNetworkReply*, QList<QSslError>)));
}

ServerWtb::~ServerWtb()
{

}

void ServerWtb::serverConnect(QString paramUrl)
{
    qDebug() << "request new connection to server";
    mainUrl=paramUrl;
    QUrl url(mainUrl);
    this->currentReply = nam->get(QNetworkRequest(url));
    nam->setCookieJar(new QNetworkCookieJar(this));
}

void ServerWtb::reqSession(QString username)
{
    qDebug() << "request session...";
    QNetworkRequest request;
    QString conString = QString("%1/C_login/get_session?param=%2").arg(mainUrl).arg(username);
    qDebug() << "connected to " << conString;
    request.setUrl(QUrl(conString));
    request.setRawHeader("User-Agent", "WTBBrowser 1.0");
    currentReply = nam->get(request);
}

void ServerWtb::finishedSlot(QNetworkReply* reply)
{
    QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);    
    QVariant redirectionTargetUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);    

    qDebug() << "* status: " << statusCodeV.toString();
    if( redirectionTargetUrl.toString() != "") {
        qDebug() << "* redirection: " << redirectionTargetUrl.toString();
    }
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "* got reply";
        QList<QByteArray> headerlist = reply->rawHeaderList();
        QByteArray header;
        for(auto it = headerlist.cbegin(); it != headerlist.cend(); ++it) {
            header = reply->rawHeader(*it);
            qDebug() << *it;
            qDebug() << header;
            if(*it == "Content-Type" && header == "application/pdf") {
                qDebug() << "File name: " << reply->rawHeader("Content-Disposition");
                QString filename = reply->rawHeader("Content-Disposition");
                filename = filename.remove("inline; filename=\"");
                filename = filename.remove("\"");
                qDebug() << "filename: " << filename ;
                QUrl urlReply = reply->url();
                qDebug() << "reply success" ;
                QString filenameFinal = saveFileName(filename);
                if(saveToDisk(filenameFinal, reply)) {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(filenameFinal));
                }
            }
        }
    }    
    else {
        qDebug() << "* error: " << reply->errorString();
    }
}

void ServerWtb::ignoreSSLErrors(QNetworkReply* reply, QList<QSslError> errors)
{
    reply->ignoreSslErrors(errors);
}

void ServerWtb::reqLogin(QString username, QString password, QString _tok)
{    
    qDebug() << "request login...";
    QNetworkRequest request;
    request.setUrl(QUrl(QString("%1/C_login").arg(mainUrl)));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    request.setRawHeader("User-Agent", "WTBBrowser 1.0");

    QUrlQuery params;
    params.addQueryItem("username", username);
    params.addQueryItem("password", password);
    params.addQueryItem("_tok", _tok);

    currentReply = nam->post(request, params.toString().toUtf8());
}

void ServerWtb::reqSubmit(QString module, QUrlQuery params)
{
    qDebug() << "request submit...";
    QNetworkRequest request;
    request.setUrl(QUrl(QString("%1/C_%2/submit").arg(mainUrl).arg(module)));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    request.setRawHeader("User-Agent", "WTBBrowser 1.0");
    currentReply = nam->post(request, params.toString().toUtf8());
}


void ServerWtb::reqHome(QUrl home)
{
    qDebug() << "request home...";
    QNetworkRequest request;
    request.setUrl(QUrl(home));
    request.setRawHeader("User-Agent", "WTBBrowser 1.0");
    currentReply = nam->get(request);
}

void ServerWtb::reqModule(QString module)
{
    qDebug() << "request module index...";
    QNetworkRequest request;
    QString conString = QString("%1/C_%2/index").arg(mainUrl).arg(module);
    request.setUrl(QUrl(conString));
    request.setRawHeader("User-Agent", "WTBBrowser 1.0");
    currentReply = nam->get(request);
}

void ServerWtb::reqAutoComplete(QString module, QString variable, QString param)
{
    qDebug() << "request autocomplete...";
    QNetworkRequest request;
    QString conString = QString("%1/C_%2/call_%3_autocomplete?term=%4").arg(mainUrl).arg(module).arg(variable).arg(param);
    QUrl url(conString);
    request.setUrl(QUrl(url.toEncoded()));
    request.setRawHeader("User-Agent", "WTBBrowser 1.0");
    currentReplyAC = nam->get(request);
}

 void ServerWtb::reqModuleGet(QString module, QString param)
 {
     qDebug() << "request module get...";
     QNetworkRequest request;
     QString conString = QString("%1/C_%2/get?%3").arg(mainUrl).arg(module).arg(param);
     QUrl url(conString);
     request.setUrl(QUrl(url.toEncoded()));
     request.setRawHeader("User-Agent", "WTBBrowser 1.0");
     currentReplyGet = nam->get(request);
 }

 void ServerWtb::reqGetController(QString module, QString controller, QString param)
 {
     qDebug() << "request get...";
     QNetworkRequest request;
     QString conString = QString("%1/C_%2/%3?%4").arg(mainUrl).arg(module).arg(controller).arg(param);
     QUrl url(conString);
     request.setUrl(QUrl(url.toEncoded()));
     request.setRawHeader("User-Agent", "WTBBrowser 1.0");
     currentReplyGet = nam->get(request);
 }


 void ServerWtb::reqPdfController(QString module, QString controller, QString param)
 {
     qDebug() << "request pdf...";
     QNetworkRequest request;
     QString conString = QString("%1/C_%2/%3?%4").arg(mainUrl).arg(module).arg(controller).arg(param);
     QUrl url(conString);
     request.setUrl(QUrl(url.toEncoded()));
     request.setRawHeader("User-Agent", "WTBBrowser 1.0");
     currentReplyPdf = nam->get(request);
 }

 bool ServerWtb::saveToDisk(const QString &filename, QIODevice *data)
 {
     QFile file(filename);
     if (!file.open(QIODevice::WriteOnly)) {
         fprintf(stderr, "Could not open %s for writing: %s\n",
                 qPrintable(filename),
                 qPrintable(file.errorString()));
         return false;
     }

     file.write(data->readAll());
     file.close();

     return true;
 }

 QString ServerWtb::saveFileName(const QUrl &url)
 {
     QString path = url.path();
     QString basename = QFileInfo(path).baseName();
     QString filename = QFileInfo(path).fileName();
     QString suffix = QFileInfo(path).completeSuffix();
     if (basename.isEmpty())
         basename = "download";

     if (QFile::exists(filename)) {
         // already exists, don't overwrite
         int i = 0;
         basename += '.';
         while (QFile::exists(basename + QString::number(i) + '.' + suffix))
             ++i;

         basename += QString::number(i);
     }

     QString result = basename+"."+suffix;

     return result;
 }

