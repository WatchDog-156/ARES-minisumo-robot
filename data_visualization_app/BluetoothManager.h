#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QObject>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QBluetoothDeviceInfo>

class BluetoothManager : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothManager(QObject *parent = nullptr);
    ~BluetoothManager();

    void connectToDevice(const QBluetoothDeviceInfo &device);
    void disconnectDevice();
    void writeData(const QByteArray &data); // Wysyłanie

signals:
    void dataReceived(const QByteArray &data); // Odbieranie
    void connectionStatusChanged(bool connected, QString message);

private slots:
    void serviceDiscovered(const QBluetoothUuid &gatt);
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void updateCharacteristicValue(const QLowEnergyCharacteristic &c, const QByteArray &value);

private:
    QLowEnergyController *controller = nullptr;
    QLowEnergyService *service = nullptr;
    QLowEnergyCharacteristic writeCharacteristic;

    const QBluetoothUuid serviceUuid = QBluetoothUuid(QString("0000ffe0-0000-1000-8000-00805f9b34fb"));
    const QBluetoothUuid charUuid = QBluetoothUuid(QString("0000ffe1-0000-1000-8000-00805f9b34fb"));
};

#endif // BLUETOOTHMANAGER_H
