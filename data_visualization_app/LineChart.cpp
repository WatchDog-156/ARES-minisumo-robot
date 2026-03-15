#include "LineChart.h"
#include "ui_LineChart.h"
#include <QVBoxLayout>

LineChart::LineChart(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LineChart)
{
    ui->setupUi(this);

    chart = new QChart();

    axisX = new QValueAxis();
    axisY = new QValueAxis();

    QList<QColor> colors = { QColor(128, 128, 0), QColor(0, 128, 128) };

    for(int i = 0; i < 2; ++i) {
        series[i] = new QLineSeries();
        series[i]->setName(QString("Line %1").arg(i + 1));
        series[i]->setColor(colors[i]);
        chart->addSeries(series[i]);
    }

    axisX->setRange(0, maxPoints);
    axisX->setTitleText("Probe");
    axisX->setLabelFormat("%d");
    axisX->setTickCount(11);

    axisY->setRange(0, 4095);
    axisY->setTitleText("Reflected light value");
    axisY->setLabelFormat("%d");
    axisY->setTickCount(11);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    for(int i = 0; i < 2; ++i) {
        series[i]->attachAxis(axisX);
        series[i]->attachAxis(axisY);
    }

    chart->setTitle("Measurement line detectors 1-2");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignTop);

    for (QLegendMarker *marker : chart->legend()->markers()) {
        connect(marker, &QLegendMarker::clicked, this, [=]() {
            QAbstractSeries *series = marker->series();
            if (!series) return;

            bool isVisible = series->isVisible();
            series->setVisible(!isVisible);
            marker->setVisible(true);

            qreal alpha = series->isVisible() ? 1.0 : 0.4;
            QColor color = marker->labelBrush().color();
            color.setAlphaF(alpha);
            marker->setLabelBrush(QBrush(color));

            QBrush brush = marker->brush();
            QColor brushColor = brush.color();
            brushColor.setAlphaF(alpha);
            brush.setColor(brushColor);
            marker->setBrush(brush);
        });
    }

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Zarządzanie layoutem
    if (this->layout() == nullptr) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0,50,0,0);
        layout->addWidget(chartView);
    } else {
        this->layout()->addWidget(chartView);
    }
}

LineChart::~LineChart()
{
    delete ui;
}

void LineChart::addMeasurement(int line_l, int line_r)
{
    int values[] = { line_l, line_r};

    for(int i = 0; i < 2; ++i) {
        dataBuffers[i].append(QPointF(iteration, values[i]));

        if (dataBuffers[i].size() > maxPoints) {
            dataBuffers[i].removeFirst();
        }

        series[i]->replace(dataBuffers[i]);
    }

    iteration++;

    if (iteration > maxPoints) {
        axisX->setRange(iteration - maxPoints, iteration);
    }
}