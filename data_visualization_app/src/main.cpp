#include "MainWindow.hpp"

#include <QApplication>
#include <QLocale>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/img/ikonka.png"));

    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
