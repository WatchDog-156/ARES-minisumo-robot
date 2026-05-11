#include "RobotDiagram.h"
#include "ui_RobotDiagram.h"
#include <QHBoxLayout>

RobotDiagram::RobotDiagram(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RobotDiagram)
{
    ui->setupUi(this);

    this->setStyleSheet("QWidget#RobotDiagram { background-color: #2b2b2b; }");

    m_cone1 = new TofConeWidget(this);
    m_cone2 = new TofConeWidget(this);
    m_cone3 = new TofConeWidget(this);
    m_cone4 = new TofConeWidget(this);
 
    m_cone1->setData(0, -30.0);
    m_cone2->setData(0, 0.0);
    m_cone3->setData(0, 0.0);
    m_cone4->setData(0, +30.0);
 
    ui->cones->setContentsMargins(4, 4, 4, 0);
    ui->cones->setSpacing(2);
    ui->cones->addWidget(m_cone1);
    ui->cones->addWidget(m_cone2);
    ui->cones->addWidget(m_cone3);
    ui->cones->addWidget(m_cone4);

    ui->distance_sensors->setFixedHeight(50);
    ui->distance_sensors->setStyleSheet("QWidget#tofBar { background-color: #3399CC; }");
    

   

    QHBoxLayout *tofLayout = qobject_cast<QHBoxLayout*>(ui->distance_sensors->layout());
    tofLayout->setContentsMargins(8, 4, 8, 4);
    tofLayout->setSpacing(6);



    std::function<RotatedLabel*(double)> makeTofLabel = [this](double angle) -> RotatedLabel* {
        RotatedLabel *tof = new RotatedLabel(this);
        tof->setText("800 mm");
        tof->setAngle(angle);
        tof->setFixedSize(76, 28);
        return tof;
    };

    m_labelTof1 = makeTofLabel(-20.0);
    m_labelTof2 = makeTofLabel(0.0);
    m_labelTof3 = makeTofLabel(0.0);
    m_labelTof4 = makeTofLabel(+20.0);

    //usuwa label z desginer i dodaje nowe
    tofLayout->replaceWidget(ui->Tof1, m_labelTof1);
    delete ui->Tof1;
    ui->Tof1 = nullptr;

    tofLayout->replaceWidget(ui->Tof2, m_labelTof2);
    delete ui->Tof2;
    ui->Tof2 = nullptr;

    tofLayout->replaceWidget(ui->Tof3, m_labelTof3);
    delete ui->Tof3;
    ui->Tof3 = nullptr;

    tofLayout->replaceWidget(ui->Tof4, m_labelTof4);
    delete ui->Tof4;
    ui->Tof4 = nullptr;

    ui->mainBody->setStyleSheet("QWidget#mainBody { background-color: #87CEEB; }");
    ui->verticalLayout->setContentsMargins(10, 10, 10, 10);
    ui->verticalLayout->setSpacing(8);

        const QString lineStyle =
        "QLabel {"
        "  background-color: #55CC00;"
        "  border: 2px solid #336600;"
        "  border-radius: 6px;"
        "  font-size: 13px;"
        "  font-weight: bold;"
        "  color: #1a3300;"
        "}";
 
    ui->LineL->setFixedSize(80, 44);
    ui->LineL->setAlignment(Qt::AlignCenter);
    ui->LineL->setStyleSheet(lineStyle);
    ui->LineL->setText("0");
 
    ui->LineR->setFixedSize(80, 44);
    ui->LineR->setAlignment(Qt::AlignCenter);
    ui->LineR->setStyleSheet(lineStyle);
    ui->LineR->setText("0");

        const QString motorStyle =
        "QLabel {"
        "  background-color: #1a2a4a;"
        "  border: 2px solid #0a0a0a;"
        "  border-radius: 10px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "  color: white;"
        "}";
 
    ui->MotorL->setFixedSize(80, 60);
    ui->MotorL->setAlignment(Qt::AlignCenter);
    ui->MotorL->setStyleSheet(motorStyle);
    ui->MotorL->setText("0 %");
 
    ui->MotorR->setFixedSize(80, 60);
    ui->MotorR->setAlignment(Qt::AlignCenter);
    ui->MotorR->setStyleSheet(motorStyle);
    ui->MotorR->setText("0 %");
 
    ui->Motors->setContentsMargins(4, 0, 4, 4);
    ui->Motors->setSpacing(6);



    m_arrowL = new MotorArrowWidget(this);
    m_arrowR = new MotorArrowWidget(this);
    m_arrowL->setFixedSize(36, 60);
    m_arrowR->setFixedSize(36, 60);
 
    ui->Motors->insertWidget(1, m_arrowL);
    ui->Motors->insertWidget(3, m_arrowR);



}

RobotDiagram::~RobotDiagram()
{
    delete ui;
}

void RobotDiagram::updateData(int MotorL, int MotorR, int LineL, int LineR, int tof1, int tof2, int tof3, int tof4){
    m_cone1->setData(tof1, -30.0);
    m_cone2->setData(tof2, 0.0);
    m_cone3->setData(tof3, 0.0);
    m_cone4->setData(tof4, 30.0);

    m_labelTof1->setText(QString("Tof1: %1 mm").arg(tof1));
    m_labelTof2->setText(QString("Tof2: %1 mm").arg(tof2));
    m_labelTof3->setText(QString("Tof3: %1 mm").arg(tof3));
    m_labelTof4->setText(QString("Tof4: %1 mm").arg(tof4));

    m_arrowL->setSpeed(MotorL);
    m_arrowR->setSpeed(MotorR);
    ui->MotorL->setText(QString("Motor left: \n%1 %").arg(qAbs(MotorL)));
    ui->MotorR->setText(QString("Motor right: \n%1 %").arg(qAbs(MotorR)));

    ui->LineL->setText(QString("Left line: \n%1").arg(LineL));
    ui->LineR->setText(QString("Right line: \n%1").arg(LineR));


    

}