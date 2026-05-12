#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "BluetoothManager.h"
#include "BluetoothLogger.h"
#include "BluetoothScanner.h"
#include "RobotDiagram.h"
#include "TofChart.h"
#include "LineChart.h"
#include "RobotPicture.h"
// #include "RoadDiagram.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void handleCommandButtons();
    void handleManualCommands();
    void handleFunctionButtons();
    // void handleLanguage(int index);
    void connectToDevice(const QBluetoothDeviceInfo &info);
    void onDataReceived(const QByteArray &data);
    void onConnectionStatusChanged(bool success, const QString &msg);

private:
    Ui::MainWindow *ui;
    void setupConnections();
    void updateButtonStates();

    RobotDiagram *robot;
    ToFChart *tof;
    LineChart *line;
    RobotPicture *robotPicture;
    // RoadDiagram *road;
    
    BluetoothManager *bluetoothManager;
    BluetoothLogger *bluetoothLogger;
    BluetoothScanner *bluetoothScanner;

    bool isConnected = false;
};
#endif // MAINWINDOW_H
