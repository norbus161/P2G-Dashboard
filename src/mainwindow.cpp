#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitializeRangeSpectrum();
    InitializePolarPlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitializeRangeSpectrum()
{
    QLineSeries *series = new QLineSeries();

    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);
    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Radar range spectrum");

    ui->range_spectrum->setChart(chart);
    ui->range_spectrum->setRenderHint(QPainter::Antialiasing);
    ui->range_spectrum->show();
}

void MainWindow::InitializePolarPlot()
{
    const qreal angularMin = -100;
    const qreal angularMax = 100;

    const qreal radialMin = -100;
    const qreal radialMax = 100;

    QScatterSeries *series1 = new QScatterSeries();
    series1->setName("scatter");
    for (int i = angularMin; i <= angularMax; i += 10)
        series1->append(i, (i / radialMax) * radialMax + 8.0);

    QSplineSeries *series2 = new QSplineSeries();
    series2->setName("spline");
    for (int i = angularMin; i <= angularMax; i += 10)
        series2->append(i, (i / radialMax) * radialMax);

    QLineSeries *series3 = new QLineSeries();
    series3->setName("star outer");
    qreal ad = (angularMax - angularMin) / 8;
    qreal rd = (radialMax - radialMin) / 3 * 1.3;
    series3->append(angularMin, radialMax);
    series3->append(angularMin + ad*1, radialMin + rd);
    series3->append(angularMin + ad*2, radialMax);
    series3->append(angularMin + ad*3, radialMin + rd);
    series3->append(angularMin + ad*4, radialMax);
    series3->append(angularMin + ad*5, radialMin + rd);
    series3->append(angularMin + ad*6, radialMax);
    series3->append(angularMin + ad*7, radialMin + rd);
    series3->append(angularMin + ad*8, radialMax);

    QLineSeries *series4 = new QLineSeries();
    series4->setName("star inner");
    ad = (angularMax - angularMin) / 8;
    rd = (radialMax - radialMin) / 3;
    series4->append(angularMin, radialMax);
    series4->append(angularMin + ad*1, radialMin + rd);
    series4->append(angularMin + ad*2, radialMax);
    series4->append(angularMin + ad*3, radialMin + rd);
    series4->append(angularMin + ad*4, radialMax);
    series4->append(angularMin + ad*5, radialMin + rd);
    series4->append(angularMin + ad*6, radialMax);
    series4->append(angularMin + ad*7, radialMin + rd);
    series4->append(angularMin + ad*8, radialMax);

    QAreaSeries *series5 = new QAreaSeries();
    series5->setName("star area");
    series5->setUpperSeries(series3);
    series5->setLowerSeries(series4);
    series5->setOpacity(0.5);

    QPolarChart *chart = new QPolarChart();
    chart->addSeries(series1);
    chart->addSeries(series2);
    chart->addSeries(series3);
    chart->addSeries(series4);
    chart->addSeries(series5);

    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
    angularAxis->setLabelFormat("%.1f");
    angularAxis->setShadesVisible(true);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setTickCount(9);
    radialAxis->setLabelFormat("%d");
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    series1->attachAxis(radialAxis);
    series1->attachAxis(angularAxis);
    series2->attachAxis(radialAxis);
    series2->attachAxis(angularAxis);
    series3->attachAxis(radialAxis);
    series3->attachAxis(angularAxis);
    series4->attachAxis(radialAxis);
    series4->attachAxis(angularAxis);
    series5->attachAxis(radialAxis);
    series5->attachAxis(angularAxis);

    radialAxis->setRange(radialMin, radialMax);
    angularAxis->setRange(angularMin, angularMax);

    chart->setTitle("Polar plot of targets");

    ui->polar_plot->setChart(chart);
    ui->polar_plot->setRenderHint(QPainter::Antialiasing);
    ui->polar_plot->show();
}

