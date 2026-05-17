/**
 * @file RotatedLabel.cpp
 * @author Jakub Borsukiewicz & Jan Farbotko
 * @brief Klasa do rysowania prostokątów czujników tof
 * @version 0.1
 * @date 2026-05-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "RotatedLabel.hpp"
#include <QPainter>

/**
 * @brief Konstuktor nowego obiektu RotatedLabel
 * 
 * @param[in] parent - wskaźnik na rodzica obiektu 
 */
RotatedLabel::RotatedLabel(QWidget *parent) 
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(100, 100);
}

/**
 * @brief Funkcja ustawiająca text widgetu
 * 
 * @param[in] text - nazwa widgetu 
 */
void RotatedLabel::setText(const QString &text) 
{ 
    m_text = text; 
    update(); 
}

/**
 * @brief Funkcja ustalająca kąt ustawienia widgetu
 * 
 * @param[in] angleDeg - kąt ustawiena widgetu w stopniach
 */
void RotatedLabel::setAngle(double angleDeg)         
{ 
    m_angle = angleDeg;  
    update(); 
}

/**
 * @brief Funkcja rysująca widgety dla czujników tof
 * 
 * Funkcja rysuje widgety pod odpowiednim kątem i z zadaną nazwą
 * @param[in] event - szczegóły zdarzenia rysowania 
 */
void RotatedLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.translate(width() / 2.0, height() / 2.0);
    p.rotate(m_angle);

    QRect r(-38, -13, 76, 26);

    p.setBrush(Qt::white);
    p.setPen(QPen(QColor(85, 85, 204), 2.0));
    p.drawRoundedRect(r, 5, 5);

    p.setPen(Qt::black);
    QFont f;
    f.setPointSize(8);
    p.setFont(f);
    p.drawText(r, Qt::AlignCenter, m_text);
}