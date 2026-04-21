/**
 * @file TofConeWidget.cpp
 * @author Jakub Borsukiewicz
 * @brief Widget stożka czujnika ToF — rysuje stożek + etykietę mm
 * @version 1.0
 * @date 2026-03-29
 */
#include "TofConeWidget.h"
#include <QFontMetrics>

TofConeWidget::TofConeWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void TofConeWidget::setData(int distanceMm, double angleDeg)
{
    m_distance = qBound(0, distanceMm, 800);
    m_angleDeg = angleDeg;
    update();
}

void TofConeWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int   labelAreaH = 22;          // miejsce na etykietę u dołu
    const double halfAngle  = 18.0;       // rozwarcie stożka w stopniach

    // Dostępna wysokość na sam stożek
    const double availH = height() - labelAreaH - 2;

    // Długość stożka: im bliżej (mały dystans) → długi stożek, im dalej → krótki
    // Minimum 15% żeby stożek był zawsze widoczny
    const double ratio    = 1.0 - qBound(0.0, m_distance / 800.0, 1.0);
    const double coneLen  = availH * (0.15 + 0.85 * ratio);

    // Wierzchołek stożka — przy dolnej krawędzi (tu jest czujnik na robocie)
    QPointF tip(width() / 2.0, height() - labelAreaH - 2);

    double rad1 = qDegreesToRadians(-90.0 + m_angleDeg - halfAngle);
    double rad2 = qDegreesToRadians(-90.0 + m_angleDeg + halfAngle);

    QPointF p1(tip.x() + coneLen * qCos(rad1), tip.y() + coneLen * qSin(rad1));
    QPointF p2(tip.x() + coneLen * qCos(rad2), tip.y() + coneLen * qSin(rad2));

    // Kolor: zielony (daleko) → czerwony (blisko)
    double t = 1.0 - (m_distance / 800.0);
    QColor fill(
        static_cast<int>(t * 220),
        static_cast<int>((1.0 - t) * 180),
        60,
        130
    );
    QColor border = fill.darker(150);
    border.setAlpha(210);

    // Rysuj stożek
    QPainterPath path;
    path.moveTo(tip);
    path.lineTo(p1);
    path.lineTo(p2);
    path.closeSubpath();

    p.setBrush(fill);
    p.setPen(QPen(border, 1.2));
    p.drawPath(path);

    // Mały kółeczko w wierzchołku (punkt czujnika)
    p.setBrush(border);
    p.setPen(Qt::NoPen);
    p.drawEllipse(tip, 3.5, 3.5);

    // --- Etykieta z wartością mm ---
    QString label = QString("%1 mm").arg(m_distance);
    QFont font;
    font.setPointSize(8);
    font.setWeight(QFont::Medium);
    p.setFont(font);

    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(label);
    textRect.setWidth(textRect.width() + 10);
    textRect.setHeight(textRect.height() + 4);
    textRect.moveCenter(QPoint(width() / 2, height() - labelAreaH / 2));

    p.setBrush(Qt::white);
    p.setPen(QPen(QColor(80, 80, 200), 1.4));
    p.drawRoundedRect(textRect, 4, 4);

    p.setPen(Qt::black);
    p.drawText(textRect, Qt::AlignCenter, label);
}