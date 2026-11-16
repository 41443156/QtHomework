#ifndef TCPFILESERVER_H
#define TCPFILESERVER_H

#include <QDialog>
#include <QtWidgets>
#include <QtNetwork>

class TCPFileServer : public QDialog
{
    Q_OBJECT

public:
    explicit TCPFileServer(QWidget *parent = nullptr);
    ~TCPFileServer() override;
private slots:
    void start();
    void acceptConnection();
    void updataServerProgress();
    void displayError(QAbstractSocket::SocketError socketError);
private:
    QProgressBar *serverProgressBar;
    QLabel *serverStatusLable;
    QPushButton *startButton;
    QPushButton *quitButton;
    QDialogButtonBox *buttonBox;
    QTcpServer tcpServer;
    QTcpSocket *tcpServerConnection;
    qint64 totalBytes;
    qint64 byteReceived;
    qint64 fileNameSize;
    QString fileName;
    QFile *localFile;
    QByteArray inBlock;

};
#endif // TCPFILESERVER_H
