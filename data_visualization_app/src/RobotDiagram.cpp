#include "RobotDiagram.hpp"
#include "ui_RobotDiagram.h"
#include <QHBoxLayout>

RobotDiagram::RobotDiagram(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RobotDiagram)
{
    ui->setupUi(this);

    this->setStyleSheet("QWidget#RobotDiagram { background-color: #2b2b2b; }");
    
 
    QHBoxLayout *conesLayout = new QHBoxLayout(ui->cones);
    conesLayout->setContentsMargins(0, 0, 0, 0);
    conesLayout->setSpacing(0);



    m_cone1 = new TofConeWidget(this);
    m_cone2 = new TofConeWidget(this);
    m_cone3 = new TofConeWidget(this);
    m_cone4 = new TofConeWidget(this);
 
    m_cone1->setData(0, -20.0);
    m_cone2->setData(0, 0.0);
    m_cone3->setData(0, 0.0);
    m_cone4->setData(0, +20.0);

    std::function<RotatedLabel*(double)> makeTofLabel = [this](double angle) -> RotatedLabel* {
        RotatedLabel *tof = new RotatedLabel(this);
        tof->setText("800 mm");
        tof->setAngle(angle);
        return tof;
    };

    m_labelTof1 = makeTofLabel(-20.0);
    m_labelTof2 = makeTofLabel(0.0);
    m_labelTof3 = makeTofLabel(0.0);
    m_labelTof4 = makeTofLabel(+20.0);

    std::function<QVBoxLayout*(TofConeWidget*, RotatedLabel*)> makePair = [this](TofConeWidget *cone, RotatedLabel *lbl) -> QVBoxLayout* {
    QVBoxLayout *v = new QVBoxLayout();
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(0);

    v->addWidget(cone, 1);

    QWidget *lblContainer = new QWidget(this);
    lblContainer->setStyleSheet("background-color: #2255AA;");
    QHBoxLayout *lh = new QHBoxLayout(lblContainer);
    lh->setContentsMargins(0, 0, 0, 0);
    lh->addWidget(lbl, 0, Qt::AlignHCenter);

    v->addWidget(lblContainer, 0);
    return v;
};


    conesLayout->addLayout(makePair(m_cone1, m_labelTof1));
    conesLayout->addLayout(makePair(m_cone2, m_labelTof2));
    conesLayout->addLayout(makePair(m_cone3, m_labelTof3));
    conesLayout->addLayout(makePair(m_cone4, m_labelTof4));

    ui->distance_sensors->setVisible(false);
    ui->distance_sensors->setFixedHeight(0);

    ui->mainBody->setStyleSheet("QWidget#mainBody { background-color: #87CEEB; }");
    ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
    ui->verticalLayout->setSpacing(0);

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
    m_arrowL->setFixedSize(50, 60);
    m_arrowR->setFixedSize(50, 60);
 
    ui->Motors->insertWidget(1, m_arrowL);
    ui->Motors->insertWidget(3, m_arrowR);



}

RobotDiagram::~RobotDiagram()
{
    delete ui;
}

void RobotDiagram::updateData(int MotorL, int MotorR, int LineL, int LineR, int tof1, int tof2, int tof3, int tof4){
    m_cone1->setData(tof1, -20.0);
    m_cone2->setData(tof2, 0.0);
    m_cone3->setData(tof3, 0.0);
    m_cone4->setData(tof4, 20.0);

    m_labelTof1->setText(QString("%1 mm").arg(tof1));
    m_labelTof2->setText(QString("%1 mm").arg(tof2));
    m_labelTof3->setText(QString("%1 mm").arg(tof3));
    m_labelTof4->setText(QString("%1 mm").arg(tof4));

    m_arrowL->setSpeed(MotorL);
    m_arrowR->setSpeed(MotorR);
    ui->MotorL->setText(QString("Motor left: \n%1 %").arg(qAbs(MotorL)));
    ui->MotorR->setText(QString("Motor right: \n%1 %").arg(qAbs(MotorR)));

    ui->LineL->setText(QString("Left line: \n%1").arg(LineL));
    ui->LineR->setText(QString("Right line: \n%1").arg(LineR));


    

}