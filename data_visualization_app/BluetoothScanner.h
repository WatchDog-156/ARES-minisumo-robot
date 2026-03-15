#ifndef BLUETOOTHSCANNER_H
#define BLUETOOTHSCANNER_H

#include <QDialog>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>

namespace Ui {
class BluetoothScanner;
}

class BluetoothScanner : public QDialog
{
    Q_OBJECT

public:
    explicit BluetoothScanner(QWidget *parent = nullptr);
    ~BluetoothScanner();

    Q_SIGNAL void deviceSelected(const QBluetoothDeviceInfo &info);
private slots:
    void on_buttonScan_clicked();
    void on_buttonConnect_clicked();
    void addDevice(const QBluetoothDeviceInfo &info);
private:
    Ui::BluetoothScanner *ui;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QList<QBluetoothDeviceInfo> foundDevices;
};

#endif // BLUETOOTHSCANNER_H
