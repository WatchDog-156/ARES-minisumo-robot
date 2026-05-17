#ifndef TRAJECTORYWIDGET_H
#define TRAJECTORYWIDGET_H

#include <QWidget>
#include <QPointF>
#include <QPainter>
#include <QtMath>
#include <QPushButton>


class TrajectoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrajectoryWidget(QWidget *parent = nullptr);
    void update(int speedL, int speedR, double dt_ms);
    void reset();

private:

    QPushButton *m_resetButton = nullptr;

    struct TrajectoryPoint {
        double x;
        double y;
        double speed;
    };

    //stan robota
    double m_x = 0.0;
    double m_y = 0.0;
    double m_theta = M_PI/2.0; //robot obrocony na polnoc (90 stopni)

    //historia punktow trajektorii
    static constexpr int MAX_POINTS = 2000;
    TrajectoryPoint m_history[MAX_POINTS];
    int m_head = 0; //nastepny punkt
    int m_count = 0; //ilos punktow aktualana

    const double m_wheelBase = 0.062; //rozstaw kol [m]
    const double m_vMax = 0.3; //max predkosc [m/s]

    //skalowanie widoku
    double m_scale = 300.0; //ilosc pikseli na metr
    double m_offsetX = 0.0;
    double m_offsetY = 0.0;

    //aktuala predkosc
    double m_currentSpeedL = 0.0;
    double m_currentSpeedR = 0.0;

    QColor speedToColor(double normalizedSpeed) const;
    QPointF worldToScreen(double x, double y) const;
    void autoScale();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
};


#endif