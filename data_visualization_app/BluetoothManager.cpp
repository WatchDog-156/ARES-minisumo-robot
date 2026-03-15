#include "BluetoothManager.h"

BluetoothManager::BluetoothManager(QObject *parent) : QObject(parent) {}

BluetoothManager::~BluetoothManager() { disconnectDevice(); }

void BluetoothManager::connectToDevice(const QBluetoothDeviceInfo &device) {
    if (controller) {
        controller->disconnectFromDevice();
        delete controller;
    }

    controller = QLowEnergyController::createCentral(device, this);

    connect(controller, &QLowEnergyController::connected, this, [this]() {
        emit connectionStatusChanged(true, "Połączono. Szukanie serwisów...");
        controller->discoverServices();
    });

    connect(controller, &QLowEnergyController::serviceDiscovered, this, &BluetoothManager::serviceDiscovered);

    controller->connectToDevice();
}

void BluetoothManager::serviceDiscovered(const QBluetoothUuid &gatt) {
    if (gatt == serviceUuid) {
        service = controller->createServiceObject(gatt, this);

        connect(service, &QLowEnergyService::stateChanged, this, &BluetoothManager::serviceStateChanged);
        connect(service, &QLowEnergyService::characteristicChanged, this, &BluetoothManager::updateCharacteristicValue);

        service->discoverDetails();
    }
}

void BluetoothManager::serviceStateChanged(QLowEnergyService::ServiceState s) {
    if (s == QLowEnergyService::RemoteServiceDiscovered) {
        writeCharacteristic = service->characteristic(charUuid);

        const QLowEnergyDescriptor notificationDesc = writeCharacteristic.descriptor(
            QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (notificationDesc.isValid()) {
            service->writeDescriptor(notificationDesc, QByteArray::fromHex("0100"));
        }
        emit connectionStatusChanged(true, "Gotowy do transmisji");
    }
}

void BluetoothManager::updateCharacteristicValue(const QLowEnergyCharacteristic &c, const QByteArray &value) {
    if (c.uuid() == charUuid) {
        emit dataReceived(value);
    }
}

void BluetoothManager::writeData(const QString &data) {
    if (service && writeCharacteristic.isValid()) {
        QByteArray bytes = data.toUtf8();
        service->writeCharacteristic(writeCharacteristic, bytes, QLowEnergyService::WriteWithoutResponse);
    }
}

void BluetoothManager::disconnectDevice() {
    if (controller) controller->disconnectFromDevice();
}