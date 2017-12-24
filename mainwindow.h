#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QMessageBox>
#include <QTextStream>
#include <QTextCodec>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnEncode_clicked();

    void on_txtCode_textChanged();

    void on_btnDecode_clicked();

    void on_actionSaveCode_triggered();
    
    void on_actionSavePlain_triggered();

    void on_actionExit_triggered();

    void on_actionOpen_triggered();

    void on_actionSaveBinary_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
