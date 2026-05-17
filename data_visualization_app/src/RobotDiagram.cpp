/**
 * @file RobotDiagram.cpp
 * @author Jakub Borsukiewicz (borsukiewiczkuba12345@gmail.com)
 * @brief Klasa do zarządzania widget'em odpowiedzialny za wyświetlanie diagramu robota
 * @version 0.1
 * @date 2026-05-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "RobotDiagram.hpp"
#include "ui_RobotDiagram.h"
#include <QHBoxLayout>

/**
 * @brief Konstruktor nowego obiektu RobotDiagram
 * 
 *  Konstruktor rysujący cały widget diagramu, na którym są przedstawione wizualnie 
 *  otrzymywane dane z sensorów odległości, czujnika wykrywania linii oraz prędkości silników
 * @param[in] parent - wskaźnik na rodzica obiektu
 */
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
 
    m_cone1->setData(800, -20.0);
    m_cone2->setData(800, 0.0);
    m_cone3->setData(800, 0.0);
    m_cone4->setData(800, +20.0);

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

    m_arrowL = ui->ArrowL;
    m_arrowR = ui->ArrowR;
    m_arrowL->setFixedSize(50, 60);
    m_arrowR->setFixedSize(50, 60);


}

/**
 * @brief Destruktor obiektu RobotDiagram
 * 
 */
RobotDiagram::~RobotDiagram()
{
    delete ui;
}

/**
 * @brief Funkcja aktualizująca wyświetlane dane z czujników
 * 
 * Funkcja aktualizuje wizualne elemnty widgetu na podstawie zmieniających się danych z czujników
 * 
 * @param[in] MotorL - prędkość lewego silnika
 * @param[in] MotorR - prędkość prawego silnika
 * @param[in] LineL - warość lewego czujnika linii
 * @param[in] LineR - wartość prawego czujnika linii
 * @param[in] tof1 - wartość czujnika odległości nr 1 (lewy skrajny)
 * @param[in] tof2 - wartość czujnika odległości nr 2 (lewy środkowy)
 * @param[in] tof3 - wartość czujnika odległości nr 3 (prawy środkowy)
 * @param[in] tof4 - wartość czujnika odległości nr 4 (prawy skrajny)
 */
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

    lastMotorL = MotorL;
    lastMotorR = MotorR;
    lastLineL = LineL;
    lastLineR = LineR;

    updateLabelsText();
}

/**
 * @brief Funkcja pomocnicza do aktualizacji danych na czujnikach
 * 
 */
void RobotDiagram::updateLabelsText() {
    ui->MotorL->setText(tr("Motor left:") + "\n" + QString::number(qAbs(lastMotorL)) + " %");
    ui->MotorR->setText(tr("Motor right:") + "\n" + QString::number(qAbs(lastMotorR)) + " %");
    
    ui->LineL->setText(tr("Left line:") + "\n" + QString::number(lastLineL));
    ui->LineR->setText(tr("Right line:") + "\n" + QString::number(lastLineR));
}

/**
 * @brief Funkcja do aktualizacji języka interfejsu graficznego
 * 
 * Funkcja reaguje na zdarzenie QEvent::LanguageChange, które jest wysyłane,
 * gdy w aplikacji zostanie zainstalowany nowy obiekt QTranslator. * 
 * @param[in] event - wskaźnik na obiekt zdarzenia
 */
void RobotDiagram::changeEvent(QEvent *event){
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this); 

        updateLabelsText();
    }
    QWidget::changeEvent(event); 
}