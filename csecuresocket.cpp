#include "csecuresocket.h"


CSecureSocket::CSecureSocket(QObject *parent) :
    QSslSocket(parent)
{

}

CSecureSocket::~CSecureSocket()
{

}

void CSecureSocket::Process(int handle)
{
    /* Emit a message indicating the address of the client. */
    setSocketDescriptor(handle);
    emit StatusMessage(tr("Incoming connection from %1.").arg(peerAddress().toString()));

    /* Set the certificate and private key. */
    setLocalCertificate("server.crt");
    setPrivateKey("server.key.insecure");

    /* Start the server encryption process and wait for it to complete. */
    startServerEncryption();
    if(waitForEncrypted())
    {
        emit StatusMessage("Session with client is now encrypted.");

        /* Respond with a basic HTTP message. */
        write("HTTP/1.1 200 OK\r\n"
              "Content-type: text/plain\r\n"              
              "\r\n"
              "Hello World!");

        emit StatusMessage("Message sent to client. Closing connection.");
    }
    else
        emit StatusMessage(tr("An error occurred: %1.").arg(errorString()));

    /* Close the connection once the data is written. */
    disconnectFromHost();
}
