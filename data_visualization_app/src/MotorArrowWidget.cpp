/**
 * @file MotorArrowWidget.cpp
 * @author Jakub Borsukiewicz (borsukiewiczkuba12345@gmail.com)
 * @brief Klasa odpowiedzialna za rysowanie i wyświetlanie dynamicznych strzałek
 * @version 0.1
 * @date 2026-05-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "MotorArrowWidget.hpp"
#include <QPainter>

/**
 * @brief Konstruktor nowego obiektu MotorArrowWidget
 * 
 * @param[in] parent - wskaźnik na rodzica obiektu 
 */
MotorArrowWidget::MotorArrowWidget(QWidget *parent) : QWidget(parent){
    setAttribute(Qt::WA_TranslucentBackground);
}

/**
 * @brief Funckja ustalająca prędkość silnika dla widget'a
 * 
 * Funkcja określa skalę wyświetlanej prędkości silnika przez widget oraz przyjmuje 
 * aktualną prędkość
 * @param[in] speed - aktualna prędkość silnika
 */
void MotorArrowWidget::setSpeed(int speed){
    m_speed = qBound(-100, speed, 100);
    update();
}

/**
 * @brief Funkcja rysująca krztałt strzałki 
 * 
 * Funkcja rysuje krztałt strzałki na podstawie otrzymywanych prędkości silnika 
 * oraz jego kierunku obrotu
 * 
 */
void MotorArrowWidget::paintEvent(QPaintEvent *){
    if (m_speed == 0) return;
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing); //gladkie krawedzie strzalki bez schodkow

    bool forward = m_speed > 0;
    double ratio = qAbs(m_speed) / 100.0;

    int red = static_cast<int>(30 + 170 * ratio);
    int green = static_cast<int>(200 - 140 * ratio);
    QColor dynamicColor(red, green, 60);

    int midX    = width() / 2;
    int fullH   = height();
    int arrowH  = static_cast<int>(fullH * ratio);
    int headH   = fullH/3;
    int stemW   = qMax(6, static_cast<int>(width() * 0.30));
    int headW   = width() - 4;

    p.setBrush(dynamicColor);
    p.setPen(Qt::NoPen);

    if (forward) {
        // Strzałka w górę — rośnie od dołu
        int topY = fullH - arrowH;
        int headBase = topY + headH;

        QPolygon head;
        head << QPoint(midX, topY)
             << QPoint(midX - headW/2, headBase)
             << QPoint(midX + headW/2, headBase);
        p.drawPolygon(head);

        p.drawRect(midX - stemW/2, headBase, stemW, fullH - headBase);
    } else {

        int topY = arrowH;
        int headBase = topY - headH;

        QPolygon head;
        head << QPoint(midX, topY)
             << QPoint(midX - headW/2, headBase)
             << QPoint(midX + headW/2, headBase);
        p.drawPolygon(head);

        p.drawRect(midX - stemW/2, 0, stemW, headBase);
    }
}