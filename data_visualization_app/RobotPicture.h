#ifndef ROBOTPICTURE_H
#define ROBOTPICTURE_H

#include <QWidget>

namespace Ui {
class RobotPicture;
}

class RobotPicture : public QWidget
{
    Q_OBJECT

public:
    explicit RobotPicture(QWidget *parent = nullptr);
    ~RobotPicture();

private:
    Ui::RobotPicture *ui;
};

#endif // ROBOTPICTURE_H
