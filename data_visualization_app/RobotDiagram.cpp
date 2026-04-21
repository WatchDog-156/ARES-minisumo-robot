/**
 * @file RobotDiagram.cpp
 * @author Jakub Borsukiewicz
 * @brief Widget widoku robota MiniSumo
 * @version 2.0
 * @date 2026-03-29
 */
#include "RobotDiagram.h"
#include "ui_RobotDiagram.h"

RobotDiagram::RobotDiagram(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RobotDiagram)
{
    ui->setupUi(this);

    // --- Stożki ToF ---
    // Jeśli w Designer masz promoted widgets o nazwach cone1..cone4,
    // możesz zamiast tego pisać: m_cone1 = ui->cone1; itd.
    // Poniżej wersja z ręcznym dodaniem do layoutu z UI.
    m_cone1 = new TofConeWidget(this);
    m_cone2 = new TofConeWidget(this);
    m_cone3 = new TofConeWidget(this);
    m_cone4 = new TofConeWidget(this);

    // Ustaw stałe kąty — odległość będzie się zmieniać przez updateData()
    m_cone1->setData(800, -45.0);
    m_cone2->setData(800, -15.0);
    m_cone3->setData(800, +15.0);
    m_cone4->setData(800, +45.0);

    // Dodaj stożki do layoutu stożków z Designer (horizontalLayout_cones)
    ui->horizontalLayout_cones->addWidget(m_cone1);
    ui->horizontalLayout_cones->addWidget(m_cone2);
    ui->horizontalLayout_cones->addWidget(m_cone3);
    ui->horizontalLayout_cones->addWidget(m_cone4);

    // --- Strzałki silników ---
    m_arrowL = new MotorArrowWidget(this);
    m_arrowR = new MotorArrowWidget(this);

    ui->horizontalLayout_motors->insertWidget(1, m_arrowL);  // po labelMotorL
    ui->horizontalLayout_motors->insertWidget(3, m_arrowR);  // przed labelMotorR
}

RobotDiagram::~RobotDiagram()
{
    delete ui;
}

void RobotDiagram::updateData(int tof1, int tof2, int tof3, int tof4,
                               int motorL, int motorR,
                               int lineL, int lineR)
{
    // Stożki ToF
    m_cone1->setData(tof1, -45.0);
    m_cone2->setData(tof2, -15.0);
    m_cone3->setData(tof3, +15.0);
    m_cone4->setData(tof4, +45.0);

    // Wartości ToF na etykietach górnej belki
    ui->labelTof1->setText(QString("%1 mm").arg(tof1));
    ui->labelTof2->setText(QString("%1 mm").arg(tof2));
    ui->labelTof3->setText(QString("%1 mm").arg(tof3));
    ui->labelTof4->setText(QString("%1 mm").arg(tof4));

    // Silniki
    m_arrowL->setSpeed(motorL);
    m_arrowR->setSpeed(motorR);
    ui->labelMotorL->setText(QString("%1 %").arg(qAbs(motorL)));
    ui->labelMotorR->setText(QString("%1 %").arg(qAbs(motorR)));

    // Czujniki linii
    updateLineLabel(ui->labelLineL, lineL);
    updateLineLabel(ui->labelLineR, lineR);
}

void RobotDiagram::updateLineLabel(QLabel *label, int value)
{
    // Wartość niska = wykrywa czarną linię
    const int threshold = 1000;
    bool onLine = (value < threshold);

    label->setText(QString::number(value));
    label->setStyleSheet(onLine
        ? "background-color: rgba(0, 200, 0, 1); border: 2px solid #336600;"
        : "background-color: rgba(200, 50, 50, 1); border: 2px solid #660000;");
}