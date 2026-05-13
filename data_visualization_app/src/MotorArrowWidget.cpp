#include "MotorArrowWidget.hpp"
#include <QPainter>

MotorArrowWidget::MotorArrowWidget(QWidget *parent) : QWidget(parent){
    setAttribute(Qt::WA_TranslucentBackground);
}

void MotorArrowWidget::setSpeed(int speed){
    m_speed = qBound(-100, speed, 100);
    update();
}

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
        int tipY     = fullH - arrowH;
        int headBase = tipY + headH;

        QPolygon head;
        head << QPoint(midX,          tipY)
             << QPoint(midX - headW/2, headBase)
             << QPoint(midX + headW/2, headBase);
        p.drawPolygon(head);

        p.drawRect(midX - stemW/2, headBase, stemW, fullH - headBase);
    } else {
        // Strzałka w dół — rośnie od góry
        int tipY     = arrowH;
        int headBase = tipY - headH;

        QPolygon head;
        head << QPoint(midX,          tipY)
             << QPoint(midX - headW/2, headBase)
             << QPoint(midX + headW/2, headBase);
        p.drawPolygon(head);

        p.drawRect(midX - stemW/2, 0, stemW, headBase);
    }
}