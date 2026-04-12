#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "BluetoothManager.h"
#include "BluetoothLogger.h"
//#include "RobotChart.h"
#include "TofChart.h"
#include "LineChart.h"
#include "RobotPicture.h"

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
private:
    Ui::MainWindow *ui;
    void setupConnections();
    void updateButtonStates();

    //RobotChart *robot;
    ToFChart *tof;
    LineChart *line;
    RobotPicture *robotPicture;
    
    BluetoothManager *bluetoothManager;
    BluetoothLogger *bluetoothLogger;
};
#endif // MAINWINDOW_H
