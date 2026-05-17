/**
 * @file TrajectoryWidget.cpp
 * @author Jakub Borsukiewicz & Jan Farbotko
 * @brief Klasa zarządzająca widget'em odpowiedzialnym za wyświetlanie trajektori robota
 * @version 0.1
 * @date 2026-05-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "TrajectoryWidget.hpp"
#include <QResizeEvent>

TrajectoryWidget::TrajectoryWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(200, 200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("background-color: white;");

    m_resetButton = new QPushButton("Reset", this);
    m_resetButton->setFixedSize(100, 50);
    m_resetButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(50, 50, 50, 180);"
        "  color: white;"
        "  border: 1px solid #888;"
        "  border-radius: 4px;"
        "  font-size: 11px;"
        "}"
        "QPushButton:hover { background-color: rgba(80, 80, 80, 200); }"
        "QPushButton:pressed { background-color: rgba(30, 30, 30, 220); }"
    );
    connect(m_resetButton, &QPushButton::clicked, this, &TrajectoryWidget::reset);

    reset();
}

void TrajectoryWidget::reset()
{
   m_x = 0.0;
   m_y = 0.0;
   m_theta = M_PI / 2.0;
   m_head = 0;
   m_count = 0;
   m_history[m_head] = {m_x, m_y, 0.0};
   m_head = 1;
   m_count = 1;
   m_currentSpeedL = 0.0;
   m_currentSpeedR = 0.0;
   m_scale = 300.0;
   QWidget::update();
}

void TrajectoryWidget::update(int speedL, int speedR, double dt_ms)
{
   m_currentSpeedL = speedL / 100.0; //zakres (-1 -> 1) do rysowania strzalki
   m_currentSpeedR = speedR / 100.0; //zakres (-1 -> 1) do rysowania strzalki

   double vL = (speedL / 100.0) * m_vMax;
   double vR = (speedR / 100.0) * m_vMax;
   double dt = dt_ms / 1000.0; //zamiana na sekundy

   double v = (vL + vR) / 2.0;
   double w = (vR - vL) / m_wheelBase;

   if(qAbs(w) < 1e-6){
       //ruch prostoliniowy
       m_x += v*qCos(m_theta) * dt;
       m_y += v*qSin(m_theta) * dt;
   } else {
       //ruch obrotowy
       double R = v/w;
       m_x += R*(qSin(m_theta + w*dt) - qSin(m_theta));
       m_y -= R*(qCos(m_theta + w*dt) - qCos(m_theta));
       m_theta += w*dt;
   }

   //normalizacja kata do [-pi, pi]
   while (m_theta > M_PI) m_theta -=2*M_PI;
   while (m_theta < -M_PI) m_theta +=2*M_PI;

   double normalizedSpeed = (qAbs(vL) + qAbs(vR)) / (2.0 * m_vMax);
   if (normalizedSpeed > 0.01){
    m_history[m_head] = {m_x, m_y, (vL+vR) / (2.0 * m_vMax)};
    m_head = (m_head + 1) % MAX_POINTS;
    if (m_count < MAX_POINTS) m_count++;
   }

   autoScale();
   QWidget::update();
}

void TrajectoryWidget::autoScale()
{
    if (m_count < 2) return;

    int firstIdx = (m_head - m_count + MAX_POINTS) % MAX_POINTS;
    double minX = m_history[firstIdx].x;
    double maxX = m_history[firstIdx].x;
    double minY = m_history[firstIdx].y;
    double maxY = m_history[firstIdx].y;

    for (int i=0; i<m_count; i++){
        int idx = (m_head - m_count + i + MAX_POINTS) % MAX_POINTS;
        minX = qMin(minX, m_history[idx].x);
        maxX = qMax(maxX, m_history[idx].x);
        minY = qMin(minY, m_history[idx].y);
        maxY = qMax(maxY, m_history[idx].y);
    }

    minX = qMin(minX, m_x);
    maxX = qMax(maxX, m_x);
    minY = qMin(minY, m_y);
    maxY = qMax(maxY, m_y);


    double rangeX = maxX - minX;
    double rangeY = maxY - minY;
    double range = qMax(qMax(rangeX, rangeY), 0.1); //min 10cm

    double margin = 0.15;
    m_scale = qMin(width(), height()) * (1.0 - 2.0*margin) / range;

    double centralX = (minX + maxX) / 2.0;
    double centralY = (minY + maxY) / 2.0;
    m_offsetX = width() / 2.0 - centralX * m_scale;
    m_offsetY = height() / 2.0 + centralY * m_scale;
}

QPointF TrajectoryWidget::worldToScreen(double x, double y) const
{
    return QPointF(m_offsetX + x*m_scale, m_offsetY - y*m_scale);
}

QColor TrajectoryWidget::speedToColor(double normalizedSpeed) const
{
    double ratio = qBound(0.0, qAbs(normalizedSpeed), 1.0);

    int r = static_cast<int>(ratio*2.0*255);
    int g = static_cast<int>((1.0 - ratio)*2.0*255);
    return QColor(qMin(r, 255),qMin(g, 255), 0);
}

void TrajectoryWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), QColor(232, 232, 232));
    //siatka

    double gridMeters = 0.1;
    double worldLeft   = (0 - m_offsetX) / m_scale;
    double worldRight  = (width() - m_offsetX) / m_scale;
    double worldTop    = (m_offsetY - height()) / m_scale;
    double worldBottom =  m_offsetY / m_scale;

    double startX = floor(worldLeft / gridMeters)*gridMeters;
    double startY = floor(worldTop / gridMeters)*gridMeters;

    p.setPen(QPen(QColor(200, 200, 200), 1, Qt::DashLine));
    for (double wx = startX; wx < worldRight; wx += gridMeters) {
        QPointF top = worldToScreen(wx, worldTop);
        QPointF bottom = worldToScreen(wx, worldBottom);
        p.drawLine(top, bottom);
    }
    for (double wy = startY; wy < worldBottom; wy += gridMeters) {
        QPointF left = worldToScreen(worldLeft,  wy);
        QPointF right = worldToScreen(worldRight, wy);
        p.drawLine(left, right);
    }

    QPointF startScreen = worldToScreen(
        m_count > 0 ? m_history[(m_head - m_count + MAX_POINTS)%MAX_POINTS].x : 0,
        m_count > 0 ? m_history[(m_head - m_count + MAX_POINTS)%MAX_POINTS].y : 0
    );
    p.setPen(QPen(QColor(100, 100, 100), 1));
    p.setBrush(QColor(150, 150, 150, 180));
    p.drawEllipse(startScreen, 6, 6);
    p.setPen(QColor(80, 80, 80));
    p.setFont(QFont("Arial", 8));
    p.drawText(startScreen + QPointF(8, -4), "START");

    //Trajektoria - segmenty
    if (m_count >= 2){
        for (int i = 1; i<m_count; i++){
            int idxPrev = (m_head - m_count + i - 1 + MAX_POINTS)%MAX_POINTS;
            int idxCurr = (m_head - m_count + i + MAX_POINTS)%MAX_POINTS;
            QPointF from = worldToScreen(m_history[idxPrev].x, m_history[idxPrev].y);
            QPointF to = worldToScreen(m_history[idxCurr].x, m_history[idxCurr].y);
            QColor color = speedToColor(m_history[idxCurr].speed);
            p.setPen(QPen(color, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.drawLine(from, to);
        }
    }

    //pozycja robota
    QPointF robotScreen = worldToScreen(m_x, m_y);

    //Obrot
    p.save();
    p.translate(robotScreen);
    p.rotate(qRadiansToDegrees(-m_theta)+90.0);

    //Nadwozie (prostokąt)
    const int RobotWidth = 24;
    const int RobotHeight = 18;
    p.setBrush(QColor(60, 60, 180, 220));
    p.setPen(QPen(Qt::black, 1.5));
    p.drawRect(-RobotWidth/2, -RobotHeight/2, RobotWidth, RobotHeight);

    //kola (jako male prostokaty)
    p.setBrush(QColor(30, 30, 30));
    p.setPen(Qt::NoPen);
    p.drawRect(-RobotWidth/2 - 4, -RobotHeight/2 + 2, 4, RobotHeight - 4); //lewe
    p.drawRect(RobotWidth/2, -RobotHeight/2 + 2, 4, RobotHeight - 4); //prawe

    //strzalka przez robotem
    double vL = m_currentSpeedL * m_vMax;
    double vR = m_currentSpeedR * m_vMax;
    double v = (vL + vR) / 2.0;

    if(qAbs(v) > 0.01){
        int arrowLength = static_cast<int>(8 + 24*qAbs(v)/m_vMax);
        int dir = (v>0) ? -1 : 1; // -1 do przodu (gora)

        p.setPen(QPen(Qt::red, 2));
        p.setBrush(Qt::red);
        int startY = dir * RobotHeight/2;
        int topY = dir * (RobotHeight/2 + arrowLength);

        //trzon
        p.drawLine(0, startY, 0, topY);

        //grot
        QPolygon arrowHead;
        
        arrowHead << QPoint(0, topY)
                  << QPoint(-4, topY - dir*7)
                  << QPoint(4, topY - dir*7);
        p.drawPolygon(arrowHead);
    }

    p.restore();
}

void TrajectoryWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_resetButton->move(width() - m_resetButton->width() - 6, 6);

    if(m_count < 2){
        m_offsetX = width() / 2.0;
        m_offsetY = height() / 2.0;
    }

    autoScale();
}