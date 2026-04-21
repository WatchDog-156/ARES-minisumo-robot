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
    bluetoothLogger = nullptr;

    ui->RobotDiagram->setCheckable(true);
    ui->TofDiagram->setCheckable(true);
    ui->LineDiagram->setCheckable(true);
    

    // robot = new RobotChart(this);
    // ui->stackedWidget->insertWidget(0, robot);
    tof = new ToFChart(this);
    ui->stackedWidget->insertWidget(1, tof);
    line = new LineChart(this);
    ui->stackedWidget->insertWidget(2, line);
    robotPicture = new RobotPicture(this);
    // ui->stackedWidget->insertWidget(4, robotPicture);
    ui->stackedWidget->addWidget(robotPicture);
    // ui->stackedWidget->setCurrentIndex(4);
    ui->stackedWidget->setCurrentWidget(robotPicture);
    robotDiagram = new RobotDiagram(this);
    ui->stackedWidget->addWidget(robotDiagram);

    setupConnections();
    this->setWindowTitle("Panel Danych Wizualnych ARES");
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
    connect(bluetoothManager, &BluetoothManager::connectionStatusChanged, this, [](bool success, const QString &msg) {
                                                                                qDebug() << "Status BLE:" << msg;});
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
    if(!button) return;

    if(button == ui->Connection){
        qDebug() << "Go to Bluetooth Scanner";
        BluetoothScanner scanner(this);
        connect(&scanner, &BluetoothScanner::deviceSelected, this, &MainWindow::connectToDevice);
        scanner.exec();
        return;
    }

    // if(button == ui->RobotDiagram){     // zmiana na ui->Logger
    //     if (!bluetoothLogger) {
    //         bluetoothLogger = new BluetoothLogger(this);
    //         bluetoothLogger->setAttribute(Qt::WA_DeleteOnClose);
    //         connect(bluetoothLogger, &QObject::destroyed, this, [this]() {bluetoothLogger = nullptr;});
    //     }
    //     bluetoothLogger->show();
    //     bluetoothLogger->raise();    
    //     bluetoothLogger->activateWindow();
    // }

    ui->RobotDiagram->setChecked(false);
    ui->TofDiagram->setChecked(false);
    ui->LineDiagram->setChecked(false);
    button->setChecked(true);

    if (button == ui->RobotDiagram) ui->stackedWidget->setCurrentWidget(robotDiagram);
    else if (button == ui->TofDiagram) ui->stackedWidget->setCurrentWidget(tof);
    else if (button == ui->LineDiagram) ui->stackedWidget->setCurrentWidget(line);


    updateButtonStates();
}

void MainWindow::updateButtonStates(){
    QList<QPushButton*> buttons = {ui->RobotDiagram, ui->TofDiagram, ui->LineDiagram};
    QStringList colors = {"52, 152, 219", "241, 196, 15", "255, 107, 129"};

    for (int i=0; i< buttons.size(); i++){
        double alpha = buttons[i]->isChecked() ? 1.0 : 0.5;
        buttons[i]->setStyleSheet(QString("background-color: rgba(%1, %2);").arg(colors[i]).arg(alpha));
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

    static const QRegularExpression re("\\d+");
    QList<int> values;
    QRegularExpressionMatchIterator i = re.globalMatch(recivedData);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        values.append(match.captured(0).toInt());
    }

    if (values.size() == 8) {
        int lines[2] = {values[0], values[1]};
        int tofs[4] = {values[2], values[3], values[4], values[5]};
        int motors[2] = {values[6], values[7]};
        qDebug() << "Sparsowana wiadomość to:" << lines[0] << ", " << lines[1] << " | " << tofs[0] << ", " << tofs[1] << ", " << tofs[2] << ", " << tofs[3] << " | " << motors[0] << ", " << motors[1];// << std::endl;
        line->addMeasurement(lines[0],lines[1]);
        tof->addMeasurement(tofs[0],tofs[1],tofs[2],tofs[3]);
    } else {
        qDebug() << "Wiadomość nie została sparsowana";
    }
}