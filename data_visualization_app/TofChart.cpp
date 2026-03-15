#include "TofChart.h"
#include "ui_TofChart.h"
#include <QVBoxLayout>

ToFChart::ToFChart(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ToFChart)
{
    ui->setupUi(this);

    chart = new QChart();

    axisX = new QValueAxis();
    axisY = new QValueAxis();

    QList<QColor> colors = { QColor(128, 0, 0), Qt::yellow, Qt::green, QColor(128, 0, 128) };

    for(int i = 0; i < 4; ++i) {
        series[i] = new QLineSeries();
        series[i]->setName(QString("TOF %1").arg(i + 1));
        series[i]->setColor(colors[i]);
        chart->addSeries(series[i]);
    }

    axisX->setRange(0, maxPoints);
    axisX->setTitleText("Probe");
    axisX->setLabelFormat("%d");

    axisY->setRange(0, 800);
    axisY->setTitleText("Distance [mm]");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    for(int i = 0; i < 4; ++i) {
        series[i]->attachAxis(axisX);
        series[i]->attachAxis(axisY);
    }

    chart->setTitle("Measurement ToF sensors 1-4");
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

ToFChart::~ToFChart() {
    delete ui;
}

void ToFChart::addMeasurement(int tof1, int tof2, int tof3, int tof4)
{
    int values[] = { tof1, tof2, tof3, tof4 };

    for(int i = 0; i < 4; ++i) {
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