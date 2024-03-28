#ifndef CSECURESOCKET_H
#define CSECURESOCKET_H

#include <QSslSocket>
#include <QHostAddress>

class CSecureSocket : public QSslSocket
{
    Q_OBJECT

public:   
    explicit CSecureSocket(QObject *parent = 0);
    ~CSecureSocket();
    void Process(int);

signals:
    void StatusMessage(QString);
};

#endif // CSECURESOCKET_H
