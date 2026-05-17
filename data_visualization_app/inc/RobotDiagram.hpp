#ifndef ROBOTDIAGRAM_H
#define ROBOTDIAGRAM_H

#include <QWidget>
#include <QLabel>
#include <QEvent>
#include "MotorArrowWidget.hpp"
#include "RotatedLabel.hpp"
#include "TofConeWidget.hpp"

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

protected:
    void changeEvent(QEvent *event) override;

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

    void updateLabelsText();
    int lastMotorR = 0;
    int lastMotorL = 0;
    int lastLineR = 0;
    int lastLineL = 0;
};

#endif