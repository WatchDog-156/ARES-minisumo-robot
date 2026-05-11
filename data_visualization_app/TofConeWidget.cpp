#include "TofConeWidget.h"
#include <QPainterPath>
#include <QColor>

TofConeWidget::TofConeWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void TofConeWidget::setData(int distanceMm, double angleDeg)
{
    m_distance = qBound(0, distanceMm, 800);
    m_angleDeg = angleDeg;
    update();  // wyzwala paintEvent
}

void TofConeWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const double maxLength = height() * 0.90;
    const double halfAngle = 20.0;
    double ratio = 1.0 - qBound(0.0, m_distance / 800.0, 1.0);
    const double coneLength = maxLength * (0.10 + 0.90 * ratio);


    QPointF top(width() / 2.0, height() - 4);

    // Dwa punkty końcowe
    double rad1 = qDegreesToRadians(-90.0 + m_angleDeg - halfAngle);
    double rad2 = qDegreesToRadians(-90.0 + m_angleDeg + halfAngle);

    QPointF p1(top.x() + coneLength * cos(rad1), top.y() + coneLength * sin(rad1));
    QPointF p2(top.x() + coneLength * cos(rad2), top.y() + coneLength * sin(rad2));

    QPainterPath path;
    path.moveTo(top);
    path.lineTo(p1);
    path.lineTo(p2);
    path.closeSubpath();

    double t = 1.0 - (m_distance / 800.0);
    QColor fill(
        static_cast<int>(t * 200),
        static_cast<int>((1.0 - t) * 180),
        50,
        120
    );
    QColor border = fill.darker(140);
    border.setAlpha(200);

    p.setBrush(fill);
    p.setPen(QPen(border, 1.2));
    p.drawPath(path);

    p.setBrush(border);
    p.setPen(Qt::NoPen);
    p.drawEllipse(top, 3.0, 3.0);

}