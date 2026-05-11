#include "MotorArrowWidget.h"
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

    int midX = width()/2;
    int midY = height()/2;

    //max polowa widgetu
    int maxHalfLength = static_cast<int>(height() * 0.4);
    int currentHalfLength = static_cast<int>(maxHalfLength*ratio);

    //wymiar strzalki
    int y_top = forward ? midY - currentHalfLength : midY + currentHalfLength;
    int y_base = midY;


    //trzon strzalki
    p.setPen(QPen(dynamicColor, 3.0, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(midX, y_base, midX, y_top);

    //grot strzalki
    int headSize = static_cast<int>(6*ratio + 4);
    int dir = forward ? 1 : -1;

    QPolygon head;
    head << QPoint(midX, y_top) << QPoint(midX - headSize, y_top + dir * headSize) << QPoint(midX + headSize, y_top + dir * headSize);

    p.setBrush(dynamicColor);
    p.setPen(Qt::NoPen);
    p.drawPolygon(head);
}