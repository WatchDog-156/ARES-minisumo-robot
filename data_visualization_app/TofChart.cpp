#include "TofChart.h"
#include "ui_TofChart.h"
#include <QVBoxLayout>

ToFChart::ToFChart(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ToFChart)
{
    ui->setupUi(this);

    chart = new QChart();

    // Kolory dla 4 czujników (Bordowy, Żółty, Zielony, Fioletowy - jak na Twoim szkicu)
    QList<QColor> colors = { QColor(128, 0, 0), Qt::yellow, Qt::green, QColor(128, 0, 128) };

    for(int i = 0; i < 4; ++i) {
        series[i] = new QLineSeries();
        series[i]->setName(QString("TOF_%1").arg(i + 1));
        series[i]->setColor(colors[i]);
        chart->addSeries(series[i]);
    }

    // Tworzenie osi
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    axisX->setRange(0, maxPoints);
    axisX->setTitleText("Próbka");
    axisY->setRange(0, 2000); // Zakładam zasięg czujnika do 2000mm, dostosuj pod siebie
    axisY->setTitleText("Dystans [mm]");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    for(int i = 0; i < 4; ++i) {
        series[i]->attachAxis(axisX);
        series[i]->attachAxis(axisY);
    }

    chart->setTitle("Pomiary czujników TOF 1-4");
    chart->legend()->setAlignment(Qt::AlignTop);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Automatyczne dopasowanie do białego prostokąta
    if (this->layout() == nullptr) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(chartView);
    } else {
        this->layout()->addWidget(chartView);
    }
}

ToFChart::~ToFChart() { delete ui; }

void ToFChart::addMeasurement(int tof1, int tof2, int tof3, int tof4)
{
    // Tablica pomocnicza, żeby obsłużyć to w pętli
    int vals[] = { tof1, tof2, tof3, tof4 };

    for(int i = 0; i < 4; ++i) {
        dataBuffers[i].append(QPointF(iteration, vals[i]));

        if (dataBuffers[i].size() > maxPoints) {
            dataBuffers[i].removeFirst();
        }

        series[i]->replace(dataBuffers[i]);
    }

    iteration++;

    // Przesuwanie osi X (efekt "płynącego" wykresu)
    if (iteration > maxPoints) {
        chart->axisX()->setRange(iteration - maxPoints, iteration);
    }
}