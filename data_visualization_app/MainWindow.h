#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "BluetoothScanner.h"
//#include "TofChart.h"

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

    // void handleLanguage(int index);
private:
    Ui::MainWindow *ui;
    void setupConnections();
    //ToFChart *tof;
};
#endif // MAINWINDOW_H
