#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <persistence1d.hpp>

#include <QFont>

const size_t CHART_TITLE_FONT_SIZE = 20;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initializeTimeDataPlot();
    initializeRangeDataPlot();
    initializeTargetDataPlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
}

void MainWindow::initializeTimeDataPlot()
{
    m_time_data_series_re_rx1 = new QLineSeries();
    m_time_data_series_im_rx1 = new QLineSeries();
    m_time_data_series_re_rx1->setName("Inphase component antenna 1");
    m_time_data_series_im_rx1->setName("Quadratur component antenna 1");

    m_time_data_series_re_rx2 = new QLineSeries();
    m_time_data_series_im_rx2 = new QLineSeries();
    m_time_data_series_re_rx2->setName("Inphase component antenna 2");
    m_time_data_series_im_rx2->setName("Quadratur component antenna 2");

    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeBlueCerulean);
    chart->setTitle("Time domain");
    chart->addSeries(m_time_data_series_re_rx1);
    chart->addSeries(m_time_data_series_im_rx1);
    chart->addSeries(m_time_data_series_re_rx2);
    chart->addSeries(m_time_data_series_im_rx2);

    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("Samples");
    axisX->setRange(0, 63);
    axisX->setLabelFormat("%d");
    chart->addAxis(axisX, Qt::AlignBottom);
    m_time_data_series_re_rx1->attachAxis(axisX);
    m_time_data_series_im_rx1->attachAxis(axisX);
    m_time_data_series_re_rx2->attachAxis(axisX);
    m_time_data_series_im_rx2->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Amplitude");
    axisY->setRange(0, 1.2);
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);
    m_time_data_series_re_rx1->attachAxis(axisY);
    m_time_data_series_im_rx1->attachAxis(axisY);
    m_time_data_series_re_rx2->attachAxis(axisY);
    m_time_data_series_im_rx2->attachAxis(axisY);

    QFont font;
    font.setPixelSize(CHART_TITLE_FONT_SIZE);
    chart->setTitleFont(font);

    ui->time_data->setChart(chart);
    ui->time_data->setRenderHint(QPainter::Antialiasing);
    ui->time_data->show();
}

void MainWindow::initializeRangeDataPlot()
{
    m_range_data_series_upper_rx1 = new QLineSeries();
    m_range_data_series_rx1 = new QAreaSeries(m_range_data_series_upper_rx1);
    m_range_data_series_rx1->setName("Antenna 1");

    m_range_data_series_upper_rx2 = new QLineSeries();
    m_range_data_series_rx2 = new QAreaSeries(m_range_data_series_upper_rx2);
    m_range_data_series_rx2->setName("Antenna 2");

    m_range_data_maximum_rx1 = new QScatterSeries();
    m_range_data_maximum_rx1->setName("All extrema");
    m_range_data_maximum_rx1->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    m_range_data_maximum_rx1->setMarkerSize(10);
    m_range_data_maximum_rx1->setBrush(Qt::red);
    m_range_data_maximum_rx1->setBorderColor(Qt::transparent);
    m_range_data_maximum_rx1->setPointLabelsVisible(true);
    m_range_data_maximum_rx1->setPointLabelsColor(Qt::white);
    m_range_data_maximum_rx1->setPointLabelsFormat("@xPoint m");

    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeBlueCerulean);
    chart->addSeries(m_range_data_series_rx1);
    chart->addSeries(m_range_data_series_rx2);
    chart->addSeries(m_range_data_maximum_rx1);
    chart->setTitle("Range spectrum");

    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("Range [m]");
    axisX->setRange(0, 10);
    axisX->setLabelFormat("%.1f");
    chart->addAxis(axisX, Qt::AlignBottom);
    m_range_data_series_rx1->attachAxis(axisX);
    m_range_data_series_rx2->attachAxis(axisX);
    m_range_data_maximum_rx1->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Magnitude");
    axisY->setRange(0, 1.0);
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);
    m_range_data_series_rx1->attachAxis(axisY);
    m_range_data_series_rx2->attachAxis(axisY);
    m_range_data_maximum_rx1->attachAxis(axisY);

    QFont font;
    font.setPixelSize(CHART_TITLE_FONT_SIZE);
    chart->setTitleFont(font);

    ui->range_data->setChart(chart);
    ui->range_data->setRenderHint(QPainter::Antialiasing);
    ui->range_data->show();
}

void MainWindow::initializeTargetDataPlot()
{
    const qreal angularMin = -180;
    const qreal angularMax = 180;

    const qreal radialMin = 0;
    const qreal radialMax = 10;

    m_target_data_series = new QScatterSeries();
    m_target_data_series->setName("Detected targets");

    QPolarChart *chart = new QPolarChart();
    chart->addSeries(m_target_data_series);

    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
    angularAxis->setLabelFormat("%d");
    angularAxis->setShadesVisible(true);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setTickCount(6);
    radialAxis->setLabelFormat("%d [m]");
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    m_target_data_series->attachAxis(radialAxis);
    m_target_data_series->attachAxis(angularAxis);

    chart->setTitle("Polar plot of targets");
    chart->setTheme(QChart::ChartThemeBlueCerulean);
    radialAxis->setRange(radialMin, radialMax);
    angularAxis->setRange(angularMin, angularMax);
    angularAxis->setTickAnchor(angularMax);

    QFont font;
    font.setPixelSize(CHART_TITLE_FONT_SIZE);
    chart->setTitleFont(font);

    ui->target_data->setChart(chart);
    ui->target_data->setRenderHint(QPainter::Antialiasing);
    ui->target_data->show();
}

void MainWindow::updateTimeData(QList<QPointF> const & re_rx1, QList<QPointF> const & im_rx1,
                                 QList<QPointF> const & re_rx2, QList<QPointF> const & im_rx2)
{
    m_time_data_series_re_rx1->clear();
    m_time_data_series_im_rx1->clear();
    m_time_data_series_re_rx2->clear();
    m_time_data_series_im_rx2->clear();

    m_time_data_series_re_rx1->append(re_rx1);
    m_time_data_series_im_rx1->append(im_rx1);
    m_time_data_series_re_rx2->append(re_rx2);
    m_time_data_series_im_rx2->append(im_rx2);
}

void MainWindow::updateRangeData(QList<QPointF> const & rx1, QList<QPointF> const & rx2)
{
    m_range_data_series_upper_rx1->clear();
    m_range_data_series_upper_rx2->clear();
    m_range_data_series_upper_rx1->append(rx1);
    m_range_data_series_upper_rx2->append(rx2);

    updateExtrema(rx1);
}

void MainWindow::updateTargetData(const QVector<Target_Info_t> &data)
{
    m_target_data_series->clear();

    for (auto & e: data)
    {
        //qDebug() << e.radius/100 << " : " << e.azimuth;
        m_target_data_series->append(e.radius/100, e.azimuth);
    }
}

void MainWindow::updateExtrema(QList<QPointF> const & rx1)
{
    std::vector<float> data;

    for (int i = 0; i < rx1.size(); i++)
    {
        data.push_back(rx1[i].y());
    }

    p1d::Persistence1D p;
    p.RunPersistence(data);
    std::vector<p1d::TPairedExtrema> extrema;
    p.GetPairedExtrema(extrema, 0.01);

    m_range_data_maximum_rx1->clear();
    for(std::vector<p1d::TPairedExtrema>::iterator it = extrema.begin(); it != extrema.end(); it++)
    {
        QPointF test(rx1[(*it).MaxIndex].x(), rx1[(*it).MaxIndex].y());
        m_range_data_maximum_rx1->append(test);
    }
}

