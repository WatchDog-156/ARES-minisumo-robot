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
        QString label = QString("%1 [%2]").arg(info.name(), info.address().toString());

        if (ui->listDevices->findItems(label, Qt::MatchExactly).isEmpty()) {
            ui->listDevices->addItem(label);
            foundDevices.append(info);
        }
    }
}

