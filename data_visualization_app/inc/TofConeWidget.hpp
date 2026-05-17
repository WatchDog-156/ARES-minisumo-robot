#ifndef TOFCONEWIDGET_H
#define TOFCONEWIDGET_H

#include <QWidget>
#include <QPainter>

class TofConeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TofConeWidget(QWidget *parent = nullptr);

    void setData(int distanceMm, double angleDeg = 0.0);

    QSize sizeHint() const override { return QSize(80, 80); }

private:
    int m_distance = 800;
    double m_angleDeg = 0.0;

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif