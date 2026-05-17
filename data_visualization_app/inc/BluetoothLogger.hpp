#ifndef BLUETOOTHLOGGER_H
#define BLUETOOTHLOGGER_H

#include <QDialog>
#include <QEvent>

namespace Ui {
class BluetoothLogger;
}

class BluetoothLogger : public QDialog
{
    Q_OBJECT

public:
    explicit BluetoothLogger(QWidget *parent = nullptr);
    ~BluetoothLogger();

    void addSendedLog(const QString &log);
    void addReceivedLog(const QString &log);

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void on_buttonReset_clicked();

private:
    Ui::BluetoothLogger *ui;
};

#endif  // BLUETOOTHLOGGER_H