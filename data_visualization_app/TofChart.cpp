/**
 * @file TofChart.cpp
 * @author Jakub Borsukiewicz (borsukiewiczkuba12345@gmail.com)
 * @brief Klasa do zarządzania widget'em odpowiedzialnym za wyświetlanie wykresów czujników odległości
 * @version 1.0
 * @date 2026-03-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "TofChart.h"
#include "ui_TofChart.h"
#include <QVBoxLayout>

/**
 * @brief Construct a new ToF Chart:: ToF Chart object
 * 
 * Construct a new ToF Chart:: ToF Chart object
 * @param parent wskażnik na rodzica obiektu
 */
ToFChart::ToFChart(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ToFChart)
{
    ui->setupUi(this);

    chart = new QChart();

    axisX = new QValueAxis();
    axisY = new QValueAxis();

    QList<QColor> colors = {QColor(128, 0, 0), Qt::yellow, Qt::green, QColor(128, 0, 128)};

    for(int i = 0; i<4; ++i){
        series[i] = new QLineSeries();
        series[i]->setName(QString("ToF %1").arg(i+1));
        series[i]->setColor(colors[i]);
        chart->addSeries(series[i]);
    }

    axisX->setRange(0, maxPoints);
    axisX->setTitleText("Probe");
    axisX->setLabelFormat("%d");
    axisX->setTickCount(11);

    axisY->setRange(0, 800);
    axisY->setTitleText("Distance [mm]");
    axisY->setLabelFormat("%d");
    axisY->setTickCount(11);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    for(int i = 0; i < 4; ++i){
        series[i]->attachAxis(axisX);
        series[i]->attachAxis(axisY);
    }

    chart->setTitle("Measurement ToF sensors 1-4");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignTop);

    for(QLegendMarker *marker : chart->legend()->markers()){
        connect(marker, &QLegendMarker::clicked, this, &ToFChart::onLegendMarkerClicked);
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
 * @brief Destroy the ToF Chart:: ToF Chart object
 * 
 * Destroy the ToF Chart:: ToF Chart object
 */
ToFChart::~ToFChart() {
    delete ui;
}

/**
 * @brief Zarządza wykresem po kliknięciu w znacznik legendy
 * 
 * Zarządza wykresem, gdy kliknięto na znacznik legendy któregoś z wykresów
 */
void ToFChart::onLegendMarkerClicked(){
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
 * @brief Funkcja dodaje pomiary odległości do wykresu
 * 
 * Funkcja dodaje pomiary odległości do wykresu
 * @param tof1 pomiar z czujnika tof1 (lewy)
 * @param tof2 pomiar z czujnika tof2 (lewy przód)
 * @param tof3 pomiar z czujnika tof3 (prawy przód)
 * @param tof4 pomiar z czujnika tof4 (prawy)
 */
void ToFChart::addMeasurement(int tof1, int tof2, int tof3, int tof4)
{
    int values[] = {tof1, tof2, tof3, tof4};

    for(int i = 0; i<4; ++i){
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