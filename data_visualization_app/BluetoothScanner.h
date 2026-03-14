#ifndef BLUETOOTHSCANNER_H
#define BLUETOOTHSCANNER_H

#include <QDialog>

namespace Ui {
class BluetoothScanner;
}

class BluetoothScanner : public QDialog
{
    Q_OBJECT

public:
    explicit BluetoothScanner(QWidget *parent = nullptr);
    ~BluetoothScanner();

private:
    Ui::BluetoothScanner *ui;
};

#endif // BLUETOOTHSCANNER_H
