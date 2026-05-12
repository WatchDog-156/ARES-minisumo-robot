#ifndef ROBOTDIAGRAM_H
#define ROBOTDIAGRAM_H

#include <QWidget>
#include <QLabel>
#include "MotorArrowWidget.h"
#include "RotatedLabel.h"
#include "TofConeWidget.h"

namespace Ui {
class RobotDiagram;
}

class RobotDiagram : public QWidget
{
    Q_OBJECT

public:
    explicit RobotDiagram(QWidget *parent = nullptr);
    ~RobotDiagram();

    void updateData(int MotorL, int MotorR, int LineL, int LineR, int tof1, int tof2, int tof3, int tof4);

private:
    Ui::RobotDiagram *ui;

    MotorArrowWidget *m_arrowL = nullptr;
    MotorArrowWidget *m_arrowR = nullptr;
    RotatedLabel *m_labelTof1 = nullptr;
    RotatedLabel *m_labelTof2 = nullptr;
    RotatedLabel *m_labelTof3 = nullptr;
    RotatedLabel *m_labelTof4 = nullptr;
    TofConeWidget *m_cone1 = nullptr;
    TofConeWidget *m_cone2 = nullptr;
    TofConeWidget *m_cone3 = nullptr;
    TofConeWidget *m_cone4 = nullptr;
};

#endif