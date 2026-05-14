/**
 * @file MainWindow.cpp
 * @author Jakub Borsukiewicz (borsukiewiczkuba12345@gmail.com)
 * @brief Tworzy główne okno aplikacji oraz zarządza działaniem przycisków
 * @version 1.0
 * @date 2026-05-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "MainWindow.hpp"
#include "./ui_MainWindow.h"
#include <QDebug>
#include <QApplication>

/**
 * @brief Inicjalizuje główne okno aplikacji, komponenty UI oraz menedżery komunikacji
 * 
 * Tworzy instancję interfejsu użytkownika, konfiguruje wykresy ToF i linii, 
 * inicjalizuje menedżera Bluetooth oraz ustawia domyślny widok aplikacji
 * @param[in] parent - wskaźnik na obiekt rodzica
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
      
    bluetoothManager = new BluetoothManager(this);
    bluetoothLogger = nullptr;
    bluetoothScanner = new BluetoothScanner(this);

    ui->RobotDiagram->setCheckable(true);
    ui->TofDiagram->setCheckable(true);
    ui->LineDiagram->setCheckable(true);
    

    robot = new RobotDiagram(this);
    ui->stackedWidget->insertWidget(0, robot);
    tof = new ToFChart(this);
    ui->stackedWidget->insertWidget(1, tof);
    line = new LineChart(this);
    ui->stackedWidget->insertWidget(2, line);
    // road = new RoadDiagram(this);
    // ui->stackedWidget->insertWidget(3, road);
    robotPicture = new RobotPicture(this);
    ui->stackedWidget->insertWidget(3, robotPicture);
    ui->stackedWidget->addWidget(robotPicture);
    ui->stackedWidget->setCurrentWidget(robotPicture);

    setupConnections();
    this->setWindowTitle(tr("ARES Visual Data Panel"));
}

/**
 * @brief Destruktor klasy MainWindow
 * 
 * Zwalnia zasoby zajmowane przez interfejs użytkownika oraz powiązane obiekty
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Funckja konfigurująca połączenia sygnałów i slotów
 * 
 * Funkcja odpowiada za połaczenia interfejsu (kliknięcie przycisku, zatwierdzenie tekstu) 
 * z odpowiednimi metodami obsługi zdarzeń
 */
void MainWindow::setupConnections(){
    connect(ui->ButtonStart, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->ButtonStop, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->ButtonForward, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->ButtonBackward, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->ButtonServoUp, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->ButtonServoDown, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->ButtonEND, &QPushButton::clicked, this, &MainWindow::handleCommandButtons);
    connect(ui->EnterText, &QLineEdit::returnPressed, this, &MainWindow::handleManualCommands);
    connect(ui->Language, &QComboBox::currentIndexChanged, this, &MainWindow::handleLanguage);
    // connect(ui->RoadDiagram, &QPushButton::clicked, this, &MainWindow::handleFunctionButtons);
    connect(ui->Log, &QPushButton::clicked, this, &MainWindow::handleFunctionButtons);
    connect(ui->Connection, &QPushButton::clicked, this, &MainWindow::handleFunctionButtons);
    connect(ui->RobotDiagram, &QPushButton::clicked, this, &MainWindow::handleFunctionButtons);
    connect(ui->TofDiagram, &QPushButton::clicked, this, &MainWindow::handleFunctionButtons);
    connect(ui->LineDiagram, &QPushButton::clicked, this, &MainWindow::handleFunctionButtons);
    connect(bluetoothManager, &BluetoothManager::dataReceived, this, &MainWindow::onDataReceived);
    connect(bluetoothManager, &BluetoothManager::connectionStatusChanged, this, &MainWindow::onConnectionStatusChanged);
    connect(bluetoothScanner, &BluetoothScanner::deviceSelected, this, &MainWindow::connectToDevice);
    
}

/**
 * @brief Funckja do wysyłania poleceń sterujących do robota przez Bluetooth
 * 
 * Funkcja odpowiada za identyfikajce klikniętego przycisku i wysłanie odpowiadającegomu kodu sterującego
 * za pomoca bluetoothManager
 */
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

/**
 * @brief Funckja do wysyłania własnych komend
 * 
 * Funckja odpowiada za wysłanie napisanego ręcznie polecenia przez bluetooth i czyści pole po wysłaniu
 */
void MainWindow::handleManualCommands(){
    QString cmd = ui->EnterText->text();
    if (cmd.isEmpty()) return;
    qDebug() << "Wyslanie komendy: " << cmd;
    bluetoothManager->writeData(cmd);
    ui->EnterText->clear();
}

/**
 * @brief Funckja do obsługi przycisków funkcyjnych
 * 
 * Funkcja odpowiada za przyciski funkcjyjne w tym: łaczenie/rozłączanie urządzenie poprzez bluetooth, zmiana okna wyświelającego dane,
 * wyświetlanie logów
 */
void MainWindow::handleFunctionButtons(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if(!button) return;

    if(button == ui->Connection){
        if(isConnected){
            qDebug() << "Disconnecting device";
            bluetoothManager->disconnectDevice();
        }else{
            qDebug() << "Go to Bluetooth Scanner";
            bluetoothScanner->exec();
        }
        return;
    }

    if(button == ui->Log){
        if (!bluetoothLogger) {
            bluetoothLogger = new BluetoothLogger(this);
            bluetoothLogger->setAttribute(Qt::WA_DeleteOnClose);
            connect(bluetoothLogger, &QObject::destroyed, this, [this]() {bluetoothLogger = nullptr;});
        }
        bluetoothLogger->show();
        bluetoothLogger->raise();    
        bluetoothLogger->activateWindow();
    }

    ui->RobotDiagram->setChecked(false);
    ui->TofDiagram->setChecked(false);
    ui->LineDiagram->setChecked(false);
    // ui->RobotPicture->setChecked(false); //Zmiana na RoadDiagram
    button->setChecked(true);

    if (button == ui->RobotDiagram) ui->stackedWidget->setCurrentIndex(0);
    else if (button == ui->TofDiagram) ui->stackedWidget->setCurrentIndex(1);
    else if (button == ui->LineDiagram) ui->stackedWidget->setCurrentIndex(2);
    // else if (button == ui->RobotPicture) ui->stackedWidget->setCurrentIndex(3); //zmiana na RoadDiagram


    updateButtonStates();
}

/**
 * @brief Funckcja aktualizująca wygląd przycisków wyboru wykresów
 * 
 * Funkcja odpowiada za zmienianie grafiki przycisków (przezroczystości i tła) w zależności od ich stanu 
 */
void MainWindow::updateButtonStates(){
    QList<QPushButton*> buttons = {ui->RobotDiagram, ui->TofDiagram, ui->LineDiagram};
    QStringList colors = {"52, 152, 219", "241, 196, 15", "255, 107, 129"};

    for (int i=0; i< buttons.size(); i++){
        double alpha = buttons[i]->isChecked() ? 1.0 : 0.5;
        buttons[i]->setStyleSheet(QString("background-color: rgba(%1, %2);").arg(colors[i]).arg(alpha));
    }


}

/**
 * @brief Funkcja do wczytywania wybranego języka aplikacji
 * 
 * @param[in] index - numer wybranego języka GUI
 */
void MainWindow::handleLanguage(int index){
    qApp->removeTranslator(&appTranslator);

    QString languageFile;
    switch(index) {
        case 0:
            languageFile = ":/i18n/data_visualization_app_en_EN.qm";
            break;
        case 1:
            languageFile = ":/i18n/data_visualization_app_pl_PL.qm";
            break;
        case 2:
            languageFile = ":/i18n/data_visualization_app_de_DE.qm";
            break;
    }
 
    if (appTranslator.load(languageFile)) {
        qApp->installTranslator(&appTranslator);
    } else {
        qDebug() << "Nie udało się załadować pliku tłumaczenia: " << languageFile;
    }
}

/**
 * @brief Funkcja do aktualizacji języka interfejsu graficznego
 * 
 * Funkcja reaguje na zdarzenie QEvent::LanguageChange, które jest wysyłane,
 * gdy w aplikacji zostanie zainstalowany nowy obiekt QTranslator. * 
 * @param[in] event - wskaźnik na obiekt zdarzenia
 */
void MainWindow::changeEvent(QEvent *event){
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        this->setWindowTitle(tr("ARES Visual Data Panel")); 
        
        if (isConnected) {
            ui->Connection->setText(tr("Connected"));
        } else{
            ui->Connection->setText(tr("Disconnected"));
        }
    }
    QMainWindow::changeEvent(event);
}

/**
 * @brief Funckja inicjuje procedurę łączenia z wybranym urzadzeniem
 * 
 * Funkcja odpowiada za rozpoczęcie próby połączenia się z urządzenie poprzez bluetooth
 * @param[in] info - dane o urządzeniu bluetooth, z którym następuje połączenie
 */
void MainWindow::connectToDevice(const QBluetoothDeviceInfo &info) {
    qDebug() << "Próba połączenia z:" << info.name();
    bluetoothManager->connectToDevice(info);
}

/**
 * @brief Funckja parsuje odebrane dane surowe i aktualizuje wykresy
 * 
 * Funkcja wyłapuje wartości liczbowe z ciągu znaków przy użyciu wyrażeń regularnych,
 * oczekuje 8 wartości: 2 dla linii, 4 dla czujników ToF oraz 2 dla silników
 * @param[in] data - Tablica bajtów odebrana bezpośrednio z modułu Bluetooth
 */
void MainWindow::onDataReceived(const QByteArray &data) {
    QString recivedData = QString::fromUtf8(data).trimmed();
    qDebug() << "Odebrano z BLE:" << recivedData;

    static const QRegularExpression re("-?\\d+");
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
        robot->updateData(motors[0], motors[1], lines[0], lines[1], tofs[0], tofs[1], tofs[2], tofs[3]);
    } else {
        qDebug() << "Wiadomość nie została sparsowana";
    }
}

/**
 * @brief Funkcja aktualizująca przycisk połączenia
 * 
 * Funkcja zmienia wygląd przycisku połączenia w zależności od statusu połączenia z urządzeniem
 * @param[in] success - informacja o połączeniu (1) /rozłączeniu (0)
 * @param[in] msg - wiadomość otrzymywana od odpowiedniego urządzenia
 */
void MainWindow::onConnectionStatusChanged(bool success, const QString &msg){
    qDebug() << "Status BLE: " << msg;

    if (success && msg == "Gotowy do transmisji") {
        isConnected = true;
        ui->Connection->setText(tr("Connected"));
        ui->Connection->setStyleSheet(
            "background-color: green; color: black; border: 2px solid black;"
            "border-radius: 5px; font-size: 12px; font-weight: bold; padding: 4px 10px;"
        );
    } else if (!success) {
        isConnected = false;
        ui->Connection->setText(tr("Disconnected"));
        ui->Connection->setStyleSheet(
            "background-color: red; color: black; border: 2px solid black;"
            "border-radius: 5px; font-size: 12px; font-weight: bold; padding: 4px 10px;"
        );
    }
}