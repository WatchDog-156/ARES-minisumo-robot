#include "BluetoothScanner.h"
#include "ui_BluetoothScanner.h"

BluetoothScanner::BluetoothScanner(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BluetoothScanner)
{
    ui->setupUi(this);

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    discoveryAgent->setLowEnergyDiscoveryTimeout(5000);

    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &BluetoothScanner::addDevice);

    this->setWindowTitle("Bluetooth Scanner");
}

BluetoothScanner::~BluetoothScanner()
{
    delete ui;
}

void BluetoothScanner::on_buttonScan_clicked()
{
    ui->listDevices->clear();
    foundDevices.clear();
    ui->buttonScan->setText("Scanning...");

    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BluetoothScanner::on_buttonConnect_clicked()
{
    int currentRow = ui->listDevices->currentRow();
    if (currentRow >= 0) {
        QBluetoothDeviceInfo selectedDevice = foundDevices.at(currentRow);
        emit deviceSelected(selectedDevice);
        accept();
    }
}


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

void BluetoothScanner::sortAndRefreshList() {
    // Sortowanie malejące po RSSI (siła sygnału)
    std::sort(foundDevices.begin(), foundDevices.end(), [](const QBluetoothDeviceInfo &a, const QBluetoothDeviceInfo &b) {
        return a.rssi() > b.rssi(); 
    });

    // Odświeżenie interfejsu
    ui->listDevices->clear();
    for (const auto &device : foundDevices) {
        QString label = QString("%1 (%2 dBm)").arg(device.name()).arg(device.rssi());
        ui->listDevices->addItem(label); 
    }
}

