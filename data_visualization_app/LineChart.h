#ifndef LINECHART_H
#define LINECHART_H

#include <QWidget>
#include <QtCharts>
#include <QList>
#include <QPointF>
#include <QLineSeries>
#include <QValueAxis>
#include <QChartView>

namespace Ui {
class LineChart;
}

class LineChart : public QWidget
{
    Q_OBJECT

public:
    explicit LineChart(QWidget *parent = nullptr);
    ~LineChart();

    void addMeasurement(int line_l, int line_r);
private:
    Ui::LineChart *ui;
    QValueAxis *axisX = nullptr;
    QValueAxis *axisY = nullptr;
    QChart *chart = nullptr;
    QChartView *chartView = nullptr;
    QLineSeries *series[2];
    QList<QPointF> dataBuffers[2];
    int iteration = 0;
    const int maxPoints = 100;
};

#endif // LINECHART_H
