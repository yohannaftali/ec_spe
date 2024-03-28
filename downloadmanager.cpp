#include "downloadmanager.h"

DownloadManager::DownloadManager()
{
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
}

void DownloadManager::doDownload(const QUrl &url)
{
    qDebug() << "start download";
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    qDebug() << "reply" ;

#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif

    currentDownloads.append(reply);
}

QString DownloadManager::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).baseName();
    qDebug() << basename;
    QString filename = QFileInfo(path).fileName();
     qDebug() << filename;
    QString suffix = QFileInfo(path).completeSuffix();
qDebug() << suffix;
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

bool DownloadManager::saveToDisk(const QString &filename, QIODevice *data)
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

void DownloadManager::execute()
{
    QStringList args = QCoreApplication::instance()->arguments();
    args.takeFirst();           // skip the first argument, which is the program's name
    if (args.isEmpty()) {
        printf("Qt Download example - downloads all URLs in parallel\n"
               "Usage: download url1 [url2... urlN]\n"
               "\n"
               "Downloads the URLs passed in the command-line to the local directory\n"
               "If the target file already exists, a .0, .1, .2, etc. is appended to\n"
               "differentiate.\n");
        QCoreApplication::instance()->quit();
        return;
    }

    foreach (QString arg, args) {
        QUrl url = QUrl::fromEncoded(arg.toLocal8Bit());
        doDownload(url);
    }
}

void DownloadManager::sslErrors(const QList<QSslError> &sslErrors)
{
#ifndef QT_NO_SSL
    foreach (const QSslError &error, sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
}

void DownloadManager::downloadFinished(QNetworkReply *reply)
{
    qDebug() << "download finished slot" ;
    QUrl url = reply->url();
    if (reply->error()) {
         qDebug() << "reply error" ;
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(reply->errorString()));
    } else {
        qDebug() << "reply success" ;
        QString filename = saveFileName(url);
        qDebug() << "filename" << filename ;
        if (saveToDisk(filename, reply)){
            qDebug() << "Download of %s succeeded (saved to %s)\n",
                   url.toEncoded().constData(), qPrintable(filename);
            QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
           /* QPrinter printer(QPrinter::HighResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(filename);
            QPrintDialog *dlg = new QPrintDialog(&printer);
            dlg->setWindowTitle(QObject::tr("Print Document"));

            if(dlg->exec() == QDialog::Accepted) {
                print(&printer);
            }
            delete dlg;*/
        }
    }
    qDebug() << "start remove current downloads" ;
    currentDownloads.removeAll(reply);
    reply->deleteLater();

    //if (currentDownloads.isEmpty())
        // all downloads finished
        //QCoreApplication::instance()->quit();

    qDebug() << "finish" ;
}

