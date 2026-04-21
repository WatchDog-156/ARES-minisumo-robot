#ifndef TOFCONEWIDGET_H
#define TOFCONEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

/**
 * @file TofConeWidget.h
 * @brief Widget rysujący stożek czujnika ToF z wartością odległości
 */
class TofConeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TofConeWidget(QWidget *parent = nullptr);

    /**
     * @param distanceMm odległość w mm (0–800)
     * @param angleDeg   kąt obrotu stożka: -45=lewy skrajny, -15=lewy środkowy,
     *                   +15=prawy środkowy, +45=prawy skrajny
     */
    void setData(int distanceMm, double angleDeg = 0.0);

    QSize sizeHint() const override { return QSize(120, 110); }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int    m_distance = 400;
    double m_angleDeg = 0.0;
};

#endif // TOFCONEWIDGET_H