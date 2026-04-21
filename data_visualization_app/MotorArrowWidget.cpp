/**
 * @file MotorArrowWidget.cpp
 * @author Jakub Borsukiewicz
 * @brief Widget strzałki kierunku silnika
 * @version 1.0
 * @date 2026-03-29
 */
#include "MotorArrowWidget.h"
#include <QPainter>

MotorArrowWidget::MotorArrowWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

void MotorArrowWidget::setSpeed(int speed)
{
    m_speed = qBound(-100, speed, 100);
    update();
}

void MotorArrowWidget::paintEvent(QPaintEvent *)
{
    if (m_speed == 0) return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    bool   forward = (m_speed > 0);
    double ratio   = qAbs(m_speed) / 100.0;

    int cx      = width() / 2;
    int mid     = height() / 2;
    int stemH   = static_cast<int>(height() * 0.45 * ratio);
    int stemW   = qMax(4, static_cast<int>(width() * 0.22));
    int headH   = static_cast<int>(height() * 0.22);
    int headW   = static_cast<int>(width() * 0.55);

    // Kolor: pomarańczowy = do przodu, zielony = do tyłu
    QColor color = forward ? QColor(255, 136, 0) : QColor(34, 187, 34);

    p.setBrush(color);
    p.setPen(Qt::NoPen);

    if (forward) {
        // Strzałka w górę
        int stemTop    = mid - stemH / 2 - headH / 2;
        int stemBottom = mid + stemH / 2 + headH / 2;
        int headTip    = stemTop;
        int headBase   = headTip + headH;

        // Trzon
        QRect stem(cx - stemW / 2, headBase, stemW, stemBottom - headBase);
        p.drawRect(stem);

        // Grot
        QPolygon head;
        head << QPoint(cx, headTip)
             << QPoint(cx - headW / 2, headBase)
             << QPoint(cx + headW / 2, headBase);
        p.drawPolygon(head);
    } else {
        // Strzałka w dół
        int stemTop    = mid - stemH / 2 - headH / 2;
        int stemBottom = mid + stemH / 2 + headH / 2;
        int headTip    = stemBottom;
        int headBase   = headTip - headH;

        QRect stem(cx - stemW / 2, stemTop, stemW, headBase - stemTop);
        p.drawRect(stem);

        QPolygon head;
        head << QPoint(cx, headTip)
             << QPoint(cx - headW / 2, headBase)
             << QPoint(cx + headW / 2, headBase);
        p.drawPolygon(head);
    }
}