/**
 * @file BluetoothScanner.cpp
 * @author Jakub Borsukiewicz (borsukiewiczkuba12345@gmail.com)
 * @brief Tworzy oraz zarządza okienkiem "Bluetooth Scanner" do wybierania urządzenia bluetooth
 * @version 1.0
 * @date 2026-03-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "BluetoothScanner.h"
#include "ui_BluetoothScanner.h"

/**
 * @brief Construct a new Bluetooth Scanner:: Bluetooth Scanner object
 * 
 * Construct a new Bluetooth Scanner:: Bluetooth Scanner object
 * @param parent wskażnik na rodzica obiektu
 */
BluetoothScanner::BluetoothScanner(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BluetoothScanner)
{
    ui->setupUi(this);

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    discoveryAgent->setLowEnergyDiscoveryTimeout(5000);

    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothScanner::addDevice);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, [this](){ui->buttonScan->setText("Scan"); ui->buttonScan->setEnabled(true);});

    this->setWindowTitle("Bluetooth Scanner");
}

/**
 * @brief Destroy the Bluetooth Scanner:: Bluetooth Scanner object
 * 
 * Destroy the Bluetooth Scanner:: Bluetooth Scanner object
 */
BluetoothScanner::~BluetoothScanner()
{
    delete ui;
}

/**
 * @brief Funkcja odpowiada za przycisk do skanowania sieci Bluetooth.
 * 
 * Funkcja odpowiada za kliknięcie w przycisk do skanowania dostępnych sieci bluetooth.
 * Zmienia napis na przycisku, blokuje go oraz włącza dicoveryAgent
 */
void BluetoothScanner::on_buttonScan_clicked()
{
    ui->listDevices->clear();
    foundDevices.clear();
    ui->buttonScan->setText("SCANNING...");
    ui->buttonScan->setEnabled(false);

    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

/**
 * @brief Funkcja odpowiada za wybór danego urządzenia. 
 * 
 * Funkcja odpowiada za kliknięcie w przycisk do wyboru danego urządzenia. 
 * Jeśli wybrano coś z listy wysyła sygnał z inforamcją o urządzeniu i zamyka okienko.
 * W przeciwnym wypadku nic się nie dzieje.
 */
void BluetoothScanner::on_buttonConnect_clicked()
{
    int currentRow = ui->listDevices->currentRow();
    if (currentRow >= 0) {
        QBluetoothDeviceInfo selectedDevice = foundDevices.at(currentRow);
        emit deviceSelected(selectedDevice);
        accept();
    }
}

/**
 * @brief Dodaje urządzenie do listy urządzeń
 * 
 * Dodaje urządzenie do listy wraz z mocą jego sygnału na odpowiednim miejscu
 * @param info dane o urządzeniu bluetooth do dodania
 */
void BluetoothScanner::addDevice(const QBluetoothDeviceInfo &info) {
    if (info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        QString label = QString("%1 [%2] (%3 dBm)").arg(info.name(), info.address().toString(), QString::number(info.rssi()));

        if (ui->listDevices->findItems(label, Qt::MatchExactly).isEmpty()) {
            ui->listDevices->addItem(label);
            foundDevices.append(info);

            sortAndRefreshList();
        }
    }
}

/**
 * @brief Sortuje urządzenia na liście po rosnącej mocy sygnału 
 * 
 * Sortuje urządzenia na liście po rosnącej mocy sygnału 
 */
void BluetoothScanner::sortAndRefreshList() {
    std::sort(foundDevices.begin(), foundDevices.end(), [](const QBluetoothDeviceInfo &a, const QBluetoothDeviceInfo &b) {
        return a.rssi() > b.rssi(); 
    });

    ui->listDevices->clear();
    for (const auto &device : foundDevices) {
        QString label = QString("%1 (%2 dBm)").arg(device.name()).arg(device.rssi());
        ui->listDevices->addItem(label); 
    }
}

