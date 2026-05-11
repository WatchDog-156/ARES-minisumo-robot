#include "RobotDiagram.h"
#include "ui_RobotDiagram.h"
#include <QHBoxLayout>

RobotDiagram::RobotDiagram(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RobotDiagram)
{
    ui->setupUi(this);

    m_arrowL = ui->ArrowL;
    m_arrowR = ui->ArrowR;
}

RobotDiagram::~RobotDiagram()
{
    delete ui;
}

void RobotDiagram::updateData(int MotorL, int MotorR, int LineL, int LineR, int tof1, int tof2, int tof3, int tof4){
    m_arrowL->setSpeed(MotorL);
    m_arrowR->setSpeed(MotorR);
    ui->MotorL->setText(QString("Motor left: %1%").arg(qAbs(MotorL)));
    ui->MotorR->setText(QString("Motor right: %1%").arg(qAbs(MotorR)));
    ui->LineL->setText(QString("Left line: %1").arg(LineL));
    ui->LineR->setText(QString("Right line: %1").arg(LineR));
    ui->Tof1->setText(QString("Tof1: %1 mm").arg(tof1));
    ui->Tof2->setText(QString("Tof2: %1 mm").arg(tof2));
    ui->Tof3->setText(QString("Tof3: %1 mm").arg(tof3));
    ui->Tof4->setText(QString("Tof4: %1 mm").arg(tof4));

}