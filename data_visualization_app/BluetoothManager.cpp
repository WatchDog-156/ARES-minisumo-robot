/**
 * @file BluetoothManager.cpp
 * @author Jakub Borsukiewicz (borsukiewiczkuba12345@gmail.com)
 * @brief Klasa do zarządzania łącznością bluetooth za pomocą protokołu Nordic UART Service
 * @version 1.1
 * @date 2026-03-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "BluetoothManager.h"


/**
 * @brief Construct a new Bluetooth Manager:: Bluetooth Manager object
 * 
 * Construct a new Bluetooth Manager:: Bluetooth Manager object
 * @param parent wskażnik na rodzica obiektu
 */
BluetoothManager::BluetoothManager(QObject *parent) : QObject(parent) {}

/**
 * @brief Destroy the Bluetooth Manager:: Bluetooth Manager object
 *  
 * Destroy the Bluetooth Manager:: Bluetooth Manager object
 */
BluetoothManager::~BluetoothManager() { disconnectDevice(); }

/**
 * @brief Zapoczątkowanie połączenie i wyszukanie serwisów
 * 
 * Funkcja zapoczątkowuje połączenie bluetooth z wybranym urządzeniem.
 * Następuje połączenie oraz wyszukanie dostępnych serwisów
 * @param device dane o urządzeniu bluetooth, z którym następuje połączenie
 */
void BluetoothManager::connectToDevice(const QBluetoothDeviceInfo &device) {
    if (controller) {
        controller->disconnectFromDevice();
        controller->deleteLater();
        controller = nullptr;
    }

    controller = QLowEnergyController::createCentral(device, this);

    connect(controller, &QLowEnergyController::connected, this, [this]() {
        emit connectionStatusChanged(true, "Połączono. Szukanie serwisów...");
        controller->discoverServices();
    });
    connect(controller, &QLowEnergyController::serviceDiscovered, this, &BluetoothManager::serviceDiscovered);
    connect(controller, &QLowEnergyController::disconnected, this, [this]() {
        emit connectionStatusChanged(false, "Urządzenie rozłączone");
    });

    controller->connectToDevice();
}

/**
 * @brief Znajdowanie konkretnych serwisów
 * 
 * Slot, który zarządza znajdowaniem konkretnych serwisów
 * @param gatt identyfikator UUID usługi
 */
void BluetoothManager::serviceDiscovered(const QBluetoothUuid &gatt) {
    if (gatt == serviceUuid) {
        service = controller->createServiceObject(gatt, this);

        connect(service, &QLowEnergyService::stateChanged, this, &BluetoothManager::serviceStateChanged);
        connect(service, &QLowEnergyService::characteristicChanged, this, &BluetoothManager::updateCharacteristicValue);

        service->discoverDetails();
    }
}

/**
 * @brief Finalizuje połączenie Bluetooth
 * 
 * Slot odpowiedzialany za finalizacje połączenia bluetooth
 * @param s informacja o aktualnym etapie usługi
 */
void BluetoothManager::serviceStateChanged(QLowEnergyService::ServiceState s) {
    if (s == QLowEnergyService::RemoteServiceDiscovered) {
        writeCharacteristic = service->characteristic(charWriteUuid);

        QLowEnergyCharacteristic readChar = service->characteristic(charReadUuid);

        if (!writeCharacteristic.isValid() || !readChar.isValid()) {
            emit connectionStatusChanged(false, "Błąd: Nie znaleziono kanałów RX/TX");
            return;
        }

        const QLowEnergyDescriptor notificationDesc = readChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        
        if (notificationDesc.isValid()) {
            service->writeDescriptor(notificationDesc, QByteArray::fromHex("0100"));
        }

        emit connectionStatusChanged(true, "Gotowy do transmisji");
    }
}

/**
 * @brief Odbieranie przysłanych wiadomości 
 * 
 * Slot odpowiada za odbieranie wiadomości przysłanych od urządzenia bluetooth
 * @param c referencja na obiekt charakterystyki
 * @param value tablica przysłanych danych binarnych
 */
void BluetoothManager::updateCharacteristicValue(const QLowEnergyCharacteristic &c, const QByteArray &value) {
    if (c.uuid() == charReadUuid) {
        emit dataReceived(value);
    }
}

/**
 * @brief Wysłanie wiadomości po Bluetooth
 * 
 * Funkcja powoduje wysłanie wiadomości do urządzenia po bluetooth
 * @param data wiadomość do wysłania
 */
void BluetoothManager::writeData(const QString &data) {
    if (service && writeCharacteristic.isValid()) {
        QByteArray bytes = (data + "\n").toUtf8();
        service->writeCharacteristic(writeCharacteristic, bytes, QLowEnergyService::WriteWithResponse);
    }
}

/**
 * @brief Rozłączenie z połączonym urządzeniem Bluetooth
 * 
 * Funkcja powoduje rozłączenie się z aktualnie połączonym urządzeniem bluetooth
 */
void BluetoothManager::disconnectDevice() {
    if (controller) 
        controller->disconnectFromDevice();
}