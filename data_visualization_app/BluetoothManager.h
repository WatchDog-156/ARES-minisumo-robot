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
    void writeData(const QString &data);

signals:
    void dataReceived(const QByteArray &data);
    void connectionStatusChanged(bool connected, QString message);

private slots:
    void serviceDiscovered(const QBluetoothUuid &gatt);
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void updateCharacteristicValue(const QLowEnergyCharacteristic &c, const QByteArray &value);

private:
    QLowEnergyController *controller = nullptr;
    QLowEnergyService *service = nullptr;
    QLowEnergyCharacteristic writeCharacteristic;

    const QBluetoothUuid serviceUuid = QBluetoothUuid(QString("6E400001-B5A3-F393-E0A9-E50E24DCCA9E"));
    const QBluetoothUuid charWriteUuid = QBluetoothUuid(QString("{6e400002-b5a3-f393-e0a9-e50e24dcca9e}"));
    const QBluetoothUuid charReadUuid = QBluetoothUuid(QString("{6e400003-b5a3-f393-e0a9-e50e24dcca9e}"));
};

#endif // BLUETOOTHMANAGER_H
