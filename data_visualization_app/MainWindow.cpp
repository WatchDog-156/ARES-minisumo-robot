#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // tof = new ToFChart(this);
    // ui->stackedWidget->addWidget(tof);
    // ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_2_clicked()
{
    qDebug() << "Działa";
    // ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_3_clicked()
{
    BluetoothScanner scanner(this);
    scanner.exec();
}

