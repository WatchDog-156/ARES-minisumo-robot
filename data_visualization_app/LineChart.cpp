/**
 * @file LineChart.cpp
 * @author Jakub Borsukiewicz (borsukiewiczkuba12345@gmail.com)
 * @brief Klasa do zarządzania widget'em odpowiedzialnym za wyświetlanie wykresów czujników białej linii
 * @version 1.0
 * @date 2026-03-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "LineChart.h"
#include "ui_LineChart.h"
#include <QVBoxLayout>

/**
 * @brief Construct a new Line Chart:: Line Chart object
 * 
 * Construct a new Line Chart:: Line Chart object
 * @param parent wskażnik na rodzica obiektu
 */
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
        series[i]->setColor(colors[i]);
        chart->addSeries(series[i]);
    }
    series[0]->setName(QString("Left detector"));
    series[1]->setName(QString("Right detector"));

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
        connect(marker, &QLegendMarker::clicked, this, &LineChart::onLegendMarkerClicked);
    }

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    if (this->layout() == nullptr) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0,50,0,0);
        layout->addWidget(chartView);
    } else {
        this->layout()->addWidget(chartView);
    }
}

/**
 * @brief Destroy the Line Chart:: Line Chart object
 * 
 * Destroy the Line Chart:: Line Chart object
 */
LineChart::~LineChart()
{
    delete ui;
}

/**
 * @brief Zarządza wykresem po kliknięciu w znacznik legendy
 * 
 * Zarządza wykresem, gdy kliknięto na znacznik legendy któregoś z wykresów
 */
void LineChart::onLegendMarkerClicked(){
    QLegendMarker* marker = qobject_cast<QLegendMarker*>(sender());
    if (!marker) return;

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
}

/**
 * @brief Funkcja dodaje pomiary białej linii do wykresu
 * 
 * Funkcja dodaje pomiary białej linii do wykresu
 * @param line_l pomiar z lewego czujnika białej linii
 * @param line_r pomiar z prawego czujnika białej linii
 */
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