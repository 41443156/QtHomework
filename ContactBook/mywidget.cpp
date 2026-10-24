#include "mywidget.h"
#include "ui_mywidget.h"
#include<QFile>
#include<QFileDialog>

MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::MyWidget)
{
    ui->setupUi(this);
    ui->contactBook->setColumnCount(4);
    QStringList colTotal;
    colTotal<<"學號"<<"班級"<<"姓名"<<"電話";
    ui->contactBook->setHorizontalHeaderLabels(colTotal);

}

MyWidget::~MyWidget()
{
    delete ui;
}
void MyWidget::on_insertButton_clicked()
{
    QTableWidgetItem *col1,*col2,*col3,*col4;
    col1=new QTableWidgetItem(ui->lineEdit_1->text());
    col2=new QTableWidgetItem(ui->lineEdit_2->text());
    col3=new QTableWidgetItem(ui->lineEdit_3->text());
    col4=new QTableWidgetItem(ui->lineEdit_4->text());
    int row =ui->contactBook->rowCount();
    qDebug()<<row;
    ui->contactBook->insertRow(row);
    ui->contactBook->setItem(row,0,col1);
    ui->contactBook->setItem(row,1,col2);
    ui->contactBook->setItem(row,2,col3);
    ui->contactBook->setItem(row,3,col4);
}

void CbWrite(QString fileName,QString str){
    QFile outFile(fileName);
    if (!outFile.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug()<<"Open File  failed!";
        return;
    }
    QTextStream out (&outFile);
    out.setEncoding(QStringConverter::System);
    out<<str;
    outFile.flush();
    outFile.close();



}




void MyWidget::on_exportButton_clicked()
{
    QString fileName;
    QString saveStr;
    fileName = QFileDialog::getSaveFileName(this,"匯出通訊錄",".","csv(*.csv);;text(*.text)");
    for(int i=0;i<ui->contactBook->rowCount();i++)
    {
        for (int j=0;j<ui->contactBook->columnCount();j++)
        {
            saveStr += ui->contactBook->item(i,j)->text()+(j<ui->contactBook->columnCount()?",":"");
        }
        saveStr +="\n";
    }
    CbWrite(fileName,saveStr);
}
// 新增：匯入按鈕點擊事件
void MyWidget::on_importButton_clicked()
{
    // 1. 開啟檔案選擇對話框
    QString fileName = QFileDialog::getOpenFileName(this, "匯入通訊錄", ".", "csv(*.csv);;text(*.text)");
    if (fileName.isEmpty())
        return;

    // 2. 開啟檔案準備讀取
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Open File failed!";
        return;
    }

    // 3. 根據需求：建置時 widget 中無資料，匯入前先清空表格現有列數
    ui->contactBook->setRowCount(0);

    // 4. 讀取內容並填入表格
    QTextStream in(&file);
    in.setEncoding(QStringConverter::System);

    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.trimmed().isEmpty())
            continue;

        // 以逗號分隔 CSV 欄位
        QStringList fields = line.split(',');
        int row = ui->contactBook->rowCount();
        ui->contactBook->insertRow(row);

        for (int j = 0; j < fields.size() && j < ui->contactBook->columnCount(); ++j)
        {
            QTableWidgetItem *item = new QTableWidgetItem(fields[j]);
            ui->contactBook->setItem(row, j, item);
        }
    }

    file.close();
}

