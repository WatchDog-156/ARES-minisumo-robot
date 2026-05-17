#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator> 
#include <QEvent>
#include "BluetoothManager.hpp"
#include "BluetoothLogger.hpp"
#include "BluetoothScanner.hpp"
#include "RobotDiagram.hpp"
#include "TofChart.hpp"
#include "LineChart.hpp"
#include "RobotPicture.hpp"
#include "TrajectoryWidget.hpp"

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

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void handleCommandButtons();
    void handleManualCommands();
    void handleFunctionButtons();
    void handleLanguage(int index);
    void connectToDevice(const QBluetoothDeviceInfo &info);
    void onDataReceived(const QByteArray &data);
    void onConnectionStatusChanged(bool success, const QString &msg);

private:
    Ui::MainWindow *ui;
    void setupConnections();
    void updateButtonStates();

    QTranslator appTranslator;

    RobotDiagram *robot;
    ToFChart *tof;
    LineChart *line;
    RobotPicture *robotPicture;
    TrajectoryWidget *road;
    
    BluetoothManager *bluetoothManager;
    BluetoothLogger *bluetoothLogger;
    BluetoothScanner *bluetoothScanner;

    bool isConnected = false;
};
#endif // MAINWINDOW_H
