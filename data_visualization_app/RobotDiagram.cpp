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

void RobotDiagram::updateData(int MotorL, int MotorR, int LineL, int LineR){
    m_arrowL->setSpeed(MotorL);
    m_arrowR->setSpeed(MotorR);
    ui->MotorL->setText(QString("Motor left: %1%").arg(qAbs(MotorL)));
    ui->MotorR->setText(QString("Motor right: %1%").arg(qAbs(MotorR)));
    ui->LineL->setText(QString("Left line: %1").arg(LineL));
    ui->LineR->setText(QString("Right line: %1").arg(LineR));

}