#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "huffman.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tblMap->setColumnWidth(0, 60);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnEncode_clicked()
{
    ui->txtCode->setPlainText(Huffman::encode(ui->txtPlain->toPlainText()));
    ui->tblMap->setRowCount(Huffman::mapping.size());
    int r = 0;
    QHashIterator<QChar, QString> it(Huffman::mapping);
    while(it.hasNext())
    {
        it.next();
        ui->tblMap->setItem(r, 0, new QTableWidgetItem(it.key()));
        ui->tblMap->setItem(r, 1, new QTableWidgetItem(it.value()));
        r++;
    }
    ui->statusBar->showMessage("已编码");
}

void MainWindow::on_txtCode_textChanged()
{
    if(ui->txtCode->toPlainText().startsWith("# Yuki Huffman Encoded"))
        ui->btnDecode->setText("<-解码");
    else
        ui->btnDecode->setText("<-解码...");
}

void MainWindow::on_btnDecode_clicked()
{
    QStringList li = ui->txtCode->toPlainText().split('\n');
    if(li[0]=="# Yuki Huffman Encoded")
    {
        Huffman::restoreTree(li[1]);
        Huffman::mapping.clear();
        Huffman::treeToMap(0, QString());
        ui->tblMap->setRowCount(Huffman::mapping.size());
        int r = 0;
        QHashIterator<QChar, QString> it(Huffman::mapping);
        while(it.hasNext())
        {
            it.next();
            ui->tblMap->setItem(r, 0, new QTableWidgetItem(it.key()));
            ui->tblMap->setItem(r, 1, new QTableWidgetItem(it.value()));
            r++;
        }
        ui->txtPlain->setPlainText(Huffman::decode(li[2]));
        ui->txtCode->setPlainText(li[2]);
        ui->statusBar->showMessage("已解码");
    }
    else
    {
        on_actionOpen_triggered();
    }
}

void MainWindow::on_actionSaveCode_triggered()
{
    QString path = QFileDialog::getSaveFileName(this, "保存编码", ".", "二进制编码(*.huffman);;编码后的文本(*.txt)");
    qDebug() << path;
    if(!path.isEmpty())
    {
        if(path.endsWith(".huffman"))
        {
            QFile file(path);
            if(!file.open(QIODevice::WriteOnly))
            {
                QMessageBox(QMessageBox::Critical, "保存文件失败", "保存二进制文件失败！", QMessageBox::Ok, this);
                return;
            }
            file.write("Yuki Huffman Encoded", 21);
            QByteArray bts = Huffman::binTreeStruct();
            int btssize = bts.size();
            file.write((char*)&btssize, sizeof(int));
            file.write(bts);
            QString code = ui->txtCode->toPlainText();
            int codesize = code.size();
            file.write((char*)&codesize, sizeof(int));
            for(int i=0; i<codesize; i+=8)
            {
                char newByte(0);
                for(int j=0; j<8 && i+j<codesize; j++)
                {
                    newByte |= (code[i+j].digitValue()<<(7-j));
                }
                file.write(&newByte, 1);
                qDebug() << (int)newByte;
            }
            file.close();
        }
        else if(path.endsWith(".txt"))
        {
            QFile file(path);
            if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QMessageBox(QMessageBox::Critical, "保存文件失败", "保存编码文件失败！", QMessageBox::Ok, this);
                return;
            }
            QTextStream ts(&file);
            ts << "# Yuki Huffman Encoded" << endl << Huffman::treeStruct() << endl << ui->txtCode->toPlainText();
            file.close();
        }
        ui->statusBar->showMessage("保存编码成功");
    }
}

void MainWindow::on_actionSavePlain_triggered()
{
    QString path = QFileDialog::getSaveFileName(this, "保存文本", ".", "文本文档(*.txt)");
    qDebug() << path;
    if(!path.isEmpty())
    {
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox(QMessageBox::Critical, "保存文件失败", "保存原文文件失败！", QMessageBox::Ok, this);
            return;
        }
        QTextStream ts(&file);
        ts.setCodec("UTF-8");
        ts << ui->txtPlain->toPlainText();
        file.close();
        ui->statusBar->showMessage("保存原文成功");
    }
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionOpen_triggered()
{
    QString path = QFileDialog::getOpenFileName(this, "打开文本或编码", ".", "支持的格式(*.txt;*.huffman)");
    qDebug() << path;
    if(!path.isEmpty())
    {
        QFile file(path);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox(QMessageBox::Critical, "打开文件失败", "无法打开选定的文件，请检查权限。", QMessageBox::Ok, this);
            return;
        }
        char tempbuff[21];
        file.read(tempbuff, 21);
        if(strcmp(tempbuff, "Yuki Huffman Encoded")==0)
        {
            int treesize;
            file.read((char*)&treesize, sizeof(int));
            char* treeStruct = new char[treesize];
            file.read(treeStruct, treesize);
            Huffman::restoreTree((uchar*)treeStruct, treesize);
            delete[] treeStruct;
            Huffman::mapping.clear();
            Huffman::treeToMap(0, QString());
            ui->tblMap->setRowCount(Huffman::mapping.size());
            int r = 0;
            QHashIterator<QChar, QString> it(Huffman::mapping);
            while(it.hasNext())
            {
                it.next();
                ui->tblMap->setItem(r, 0, new QTableWidgetItem(it.key()));
                ui->tblMap->setItem(r, 1, new QTableWidgetItem(it.value()));
                r++;
            }
            QString code;
            int codesize;
            file.read((char*)&codesize, sizeof(int));
            QByteArray binCode = file.readAll();
            for(int i=0; i*8<codesize; i++)
            {
                for(int j=0; j<8&&i*8+j<codesize; j++)
                {
                    code+=(binCode[i]&(1<<(7-j))?'1':'0');
                }
            }
            ui->txtCode->setPlainText(code);
            ui->txtPlain->setPlainText(Huffman::decode(code));
            ui->statusBar->showMessage("已打开编码文件并解码成功");
        }
        else
        {
            file.close();
            // old
            if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QMessageBox(QMessageBox::Critical, "打开文件失败", "无法打开选定的文件，请检查权限。", QMessageBox::Ok, this);
                return;
            }
            QString text(file.readAll());
            if(text.startsWith("# Yuki Huffman Encoded"))
            {
                QStringList li = text.split('\n');
                Huffman::restoreTree(li[1]);
                Huffman::mapping.clear();
                Huffman::treeToMap(0, QString());
                ui->tblMap->setRowCount(Huffman::mapping.size());
                int r = 0;
                QHashIterator<QChar, QString> it(Huffman::mapping);
                while(it.hasNext())
                {
                    it.next();
                    ui->tblMap->setItem(r, 0, new QTableWidgetItem(it.key()));
                    ui->tblMap->setItem(r, 1, new QTableWidgetItem(it.value()));
                    r++;
                }
                ui->txtPlain->setPlainText(Huffman::decode(li[2]));
                ui->txtCode->setPlainText(li[2]);
                ui->statusBar->showMessage("已打开编码文件并解码成功");
            }
            else
            {
                ui->txtPlain->setPlainText(text);
                ui->txtCode->setPlainText(Huffman::encode(text));
                ui->tblMap->setRowCount(Huffman::mapping.size());
                int r = 0;
                QHashIterator<QChar, QString> it(Huffman::mapping);
                while(it.hasNext())
                {
                    it.next();
                    ui->tblMap->setItem(r, 0, new QTableWidgetItem(it.key()));
                    ui->tblMap->setItem(r, 1, new QTableWidgetItem(it.value()));
                    r++;
                }
                ui->statusBar->showMessage("已打开文本并编码");
            }
        }
    }
}

void MainWindow::on_actionSaveBinary_triggered()
{
    QString path = QFileDialog::getSaveFileName(this, "保存二进制", ".", "二进制编码(*.huffman)");
    qDebug() << path;
    if(!path.isEmpty())
    {

    }
}
