#include "tcpfilesender.h"

TcpFileSender::TcpFileSender(QWidget *parent)
    : QDialog(parent)
{
    loadSize=1024*4;
    totalBytes=0;
    bytesWritten=0;
    bytesToWrite=0;
    clientProgressBar=new QProgressBar;
    clientStatusLabel=new QLabel(tr("客戶端就緒"));
    startButton=new QPushButton(tr("開始"));
    quitButton=new QPushButton(tr("退出"));
    openButton =new QPushButton(tr("開檔"));
    startButton->setEnabled(false);
    buttonBox=new QDialogButtonBox;
    buttonBox->addButton(startButton,QDialogButtonBox::ActionRole);
    buttonBox->addButton(openButton,QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton,QDialogButtonBox::ActionRole);

    QVBoxLayout *mainLayout =new QVBoxLayout;
    mainLayout->addWidget(clientProgressBar);
    mainLayout->addWidget(clientStatusLabel);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    setWindowTitle(tr("TCP檔案傳送"));
    connect(openButton, &QPushButton::clicked, this, &TcpFileSender::openFile);
    connect(startButton, &QPushButton::clicked, this, &TcpFileSender::start);
    connect(&tcpClient, &QTcpSocket::connected, this, &TcpFileSender::startTransfer);
    connect(&tcpClient, &QTcpSocket::bytesWritten, this, &TcpFileSender::updateClientProgress);
    connect(quitButton, &QPushButton::clicked, this, &TcpFileSender::close);


}

TcpFileSender::~TcpFileSender() = default;

void TcpFileSender::start()
{
    startButton->setEnabled(false);
    bytesWritten=0;
    clientStatusLabel->setText(tr("連接中..."));
    tcpClient.connectToHost(QHostAddress::LocalHost,16689);

}
void TcpFileSender::startTransfer()
{
    localFile=new QFile(fileName);
    if(!localFile->open(QFile::ReadOnly)){
        QMessageBox::warning(this,tr("應用程式"),tr("無法讀取%1:\n%2.").arg(fileName).arg(localFile->errorString()));
        return;
    }
    totalBytes=localFile->size();
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);
    QString currentFile=fileName.right(fileName.size()-fileName.lastIndexOf("/")-1);
    sendOut<<qint64(0)<<qint64(0)<<currentFile;
    totalBytes+=outBlock.size();
    sendOut.device()->seek(0);
    sendOut<<totalBytes<<qint64((outBlock.size()-sizeof(qint64)*2));
    bytesToWrite=totalBytes-tcpClient.write(outBlock);
    clientStatusLabel->setText(tr("已連接"));
    qDebug()<<currentFile<<totalBytes;
    outBlock.resize(0);
}
void TcpFileSender::openFile()
{
    fileName=QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()){
        startButton->setEnabled(true);
    }

}
void TcpFileSender::updateClientProgress(qint64 numberBytes)
{
    bytesWritten+=(int)numberBytes;
    if(bytesToWrite>0){
        outBlock=localFile->read(qMin(bytesToWrite,loadSize));
        bytesToWrite-=(int)tcpClient.write(outBlock);
        outBlock.resize(0);
    }
    else{
        localFile->close();
    }
    clientProgressBar->setMaximum(totalBytes);
    clientProgressBar->setValue(bytesWritten);
    clientProgressBar->setFormat(tr("已傳送%1 Bytes").arg(bytesWritten));
}