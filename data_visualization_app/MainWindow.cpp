#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include <QDebug>
#include "BluetoothScanner.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bluetoothManager = new BluetoothManager(this);

    // tof = new ToFChart(this);
    // ui->stackedWidget->addWidget(tof);
     tof = new ToFChart(this);
     ui->stackedWidget->insertWidget(1, tof);
    // tof = new ToFChart(this);
    // ui->stackedWidget->addWidget(tof);
    // ui->stackedWidget->setCurrentIndex(0);

    setupConnections();
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
    connect(ui->Connection, &QPushButton::clicked, this, &MainWindow::handleFunctionButtons);
    connect(ui->RobotDiagram, &QPushButton::clicked, this, &MainWindow::handleFunctionButtons);
    connect(ui->TofDiagram, &QPushButton::clicked, this, &MainWindow::handleFunctionButtons);
    connect(ui->LineDiagram, &QPushButton::clicked, this, &MainWindow::handleFunctionButtons);
    connect(bluetoothManager, &BluetoothManager::dataReceived, this, &MainWindow::onDataReceived);
}


void MainWindow::handleCommandButtons(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    if(button == ui->ButtonForward){
        qDebug() << "Forward";
        bluetoothManager->writeData("FWD");
    } else if (button == ui->ButtonBackward){
        qDebug() << "Backward";
        bluetoothManager->writeData("BWD");
    } else if (button == ui->ButtonStart){
        qDebug() << "Start";
        bluetoothManager->writeData("START");
    } else if (button == ui->ButtonStop){
        qDebug() << "Stop";
        bluetoothManager->writeData("STOP");
    } else if (button == ui->ButtonServoUp){
        qDebug() << "ServoUp";
        bluetoothManager->writeData("SER_UP");
    } else if (button == ui->ButtonServoDown){
        qDebug() << "ServoDown";
        bluetoothManager->writeData("SER_DW");
    } else if (button == ui->ButtonEND){
        qDebug() << "End";
        bluetoothManager->writeData("END");
    }
}

void MainWindow::handleManualCommands(){
    QString cmd = ui->EnterText->text();
    if (cmd.isEmpty()) return;
    qDebug() << "Wyslanie komendy: " << cmd;
    bluetoothManager->writeData(cmd);
    ui->EnterText->clear();
}

void MainWindow::handleFunctionButtons(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    if(button == ui->Connection){
        qDebug() << "Go to Bluetooth Scanner";
        BluetoothScanner scanner(this);
        connect(&scanner, &BluetoothScanner::deviceSelected, this, &MainWindow::connectToDevice);
        scanner.exec();
    } else if (button == ui->RobotDiagram){
        qDebug() << "Change to Robot Diagram";
        ui->stackedWidget->setCurrentIndex(0);
    } else if (button == ui->TofDiagram){
        qDebug() << "Change to ToF Diagram";
        ui->stackedWidget->setCurrentIndex(1);
    } else if (button == ui->LineDiagram){
        qDebug() << "Change to Line Diagram";
        //ui->stackedWidget->setCurrentIndex(2);
    }
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

void MainWindow::connectToDevice(const QBluetoothDeviceInfo &info) {
    qDebug() << "Próba połączenia z:" << info.name();
    bluetoothManager->connectToDevice(info);
}

void MainWindow::onDataReceived(const QByteArray &data) {
    QString recivedData = QString::fromUtf8(data).trimmed();
    qDebug() << "Odebrano z BLE:" << recivedData;
    // parser wiadomości
    // Tutaj możesz np. wywołać ui->tofChart->addMeasurement(...)
}