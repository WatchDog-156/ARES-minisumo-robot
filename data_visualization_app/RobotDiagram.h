#ifndef ROBOTDIAGRAM_H
#define ROBOTDIAGRAM_H

#include <QWidget>
#include <QLabel>
#include "MotorArrowWidget.h"

namespace Ui {
class RobotDiagram;
}

class RobotDiagram : public QWidget
{
    Q_OBJECT

public:
    explicit RobotDiagram(QWidget *parent = nullptr);
    ~RobotDiagram();

    void updateData(int MotorL, int MotorR, int LineL, int LineR);

private:
    Ui::RobotDiagram *ui;

    MotorArrowWidget *m_arrowL = nullptr;
    MotorArrowWidget *m_arrowR = nullptr;

};

#endif