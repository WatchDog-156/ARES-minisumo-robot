/**
 * @file TofConeWidget.cpp
 * @author Jakub Borsukiewicz & Jan Farbotko
 * @brief Klasa odpowiedzialna za rysowanie i wyświetlanie dynamicznych stożków
 * @version 0.1
 * @date 2026-05-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "TofConeWidget.hpp"
#include <QPainterPath>
#include <QColor>

/**
 * @brief Konstruktor nowego obiektu TofConeWidget
 * 
 * @param[in] parent - wskaźnik na rodzica obiektu 
 */
TofConeWidget::TofConeWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

/**
 * @brief Funkcja przyjmująca wartość czujnika i ustalająca kąt
 * 
 * Funkcja przyjmuje wartość wysyłaną przez czujnik i ustalająca kąt rysowania stożków
 * 
 * @param[in] distanceMm - wartość długości odczytywanej z czujnika w mm
 * @param[in] angleDeg - kąt rysowania stożka w stopniach
 */
void TofConeWidget::setData(int distanceMm, double angleDeg)
{
    m_distance = qBound(0, distanceMm, 800);
    m_angleDeg = angleDeg;
    update();
}

/**
 * @brief Funkcja rysująca stożek
 * 
 * Funkcja rysuje i wyświetla widget stożka na podstawie wartości
 *  odległości z czujnika oraz zadanego kąta
 */
void TofConeWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const double maxLength = height() * 0.65;
    const double halfAngle = 20.0;
    double ratio = qBound(0.0, m_distance / 800.0, 1.0);
    const double coneLength = maxLength * (0.10 + 0.90 * ratio);


    QPointF top(width() / 2.0, height() - 4);

    double rad1 = qDegreesToRadians(-90.0 + m_angleDeg - halfAngle);
    double rad2 = qDegreesToRadians(-90.0 + m_angleDeg + halfAngle);

    QPointF p1(top.x() + coneLength * cos(rad1), top.y() + coneLength * sin(rad1));
    QPointF p2(top.x() + coneLength * cos(rad2), top.y() + coneLength * sin(rad2));

    QPainterPath path;
    path.moveTo(top);
    path.lineTo(p1);
    path.lineTo(p2);
    path.closeSubpath();

    int red = static_cast<int>((1.0 - ratio) * 220);
    int green = static_cast<int>(ratio * 180);

    QColor fill(red, green, 50, 200);
    QColor border = fill.darker(140);
    border.setAlpha(255);

    p.setBrush(fill);
    p.setPen(QPen(border, 1.2));
    p.drawPath(path);

    p.setBrush(border);
    p.setPen(Qt::NoPen);
    p.drawEllipse(top, 3.0, 3.0);

}