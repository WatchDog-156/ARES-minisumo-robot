#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>

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
};
#endif // MainWindow_H
