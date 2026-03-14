#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_2_clicked()
{
    qDebug() << "Działa";
}


void MainWindow::on_pushButton_3_clicked()
{
    BluetoothScanner scanner(this);
    scanner.exec();
}

