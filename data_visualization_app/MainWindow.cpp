#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupConnections();

    // tof = new ToFChart(this);
    // ui->stackedWidget->addWidget(tof);
    // ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections(){
    connect(ui->ButtonStart, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->ButtonStop, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->ButtonForward, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->ButtonBackward, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->ButtonServoUp, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->ButtonServoDown, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->ButtonEND, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->EnterText, &QLineEdit::returnPressed, this, &MainWindow::handleManualCommands);
    // connect(ui->Language, &QComboBox::activated, this, &MainWindow::handleLanguage);
}


void MainWindow::handleCommandButtons(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    if(button == ui->ButtonForward){
        qDebug() << "Forward";
    } else if (button == ui->ButtonBackward){
        qDebug() << "Backward";
    } else if (button == ui->ButtonStart){
        qDebug() << "Start";
    } else if (button == ui->ButtonStop){
        qDebug() << "Stop";
    } else if (button == ui->ButtonServoUp){
        qDebug() << "ServoUp";
    } else if (button == ui->ButtonServoDown){
        qDebug() << "ServoDown";
    } else if (button == ui->ButtonEND){
        qDebug() << "End";
    }
}

void MainWindow::handleManualCommands(){
    QString cmd = ui->EnterText->text();
    if (cmd.isEmpty()) return;
    qDebug() << "Wyslanie komendy: " << cmd;
    ui->EnterText->clear();
}


// void MainWindow::handleLanguage(int index){
//     switch(index){
//         case 0:
//             qDebug() << "Angielski";
//             ui->ButtonForward->setText("Forward");
//             ui->ButtonBackward->setText("Backward");
//             break;
//         case 1:
//             qDebug() << "Polski";
//             ui->ButtonForward->setText("Przod");
//             ui->ButtonBackward->setText("Tyl");
//             break;
//         }
//     }

//void MainWindow::on_pushButton_3_clicked() connection button
//{
//    BluetoothScanner scanner(this);
//    scanner.exec();
//}

