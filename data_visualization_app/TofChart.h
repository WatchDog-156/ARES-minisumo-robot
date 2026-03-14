#ifndef TOFCHART_H
#define TOFCHART_H

#include <QWidget>
#include <QtCharts>
#include <QList>
#include <QPointF>

namespace Ui {
class ToFChart;
}

class ToFChart : public QWidget
{
    Q_OBJECT

public:
    explicit ToFChart(QWidget *parent = nullptr);
    ~ToFChart();

    void addMeasurement(int tof1, int tof2, int tof3, int tof4);
private:
    Ui::ToFChart *ui;
    QChart *chart;
    QChartView *chartView;
    QLineSeries *series[4];
    QList<QPointF> dataBuffers[4];
    int iteration = 0;
    const int maxPoints = 100;
};

#endif // TOFCHART_H
