#include "mainwindow.h"
#include "./ui_mainwindow.h"

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
    chart->addSeries(m_time_data_series_re_rx1);
    chart->addSeries(m_time_data_series_im_rx1);
    chart->addSeries(m_time_data_series_re_rx2);
    chart->addSeries(m_time_data_series_im_rx2);

    chart->setTitle("Time domain");
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).back()->setRange(0, 63);
    chart->axes(Qt::Vertical).back()->setRange(0, 1.2);
    chart->axes(Qt::Horizontal).back()->setTitleText("Samples");
    chart->axes(Qt::Vertical).back()->setTitleText("Amplitude");

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
    m_range_data_maximum_rx1->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    m_range_data_maximum_rx1->setMarkerSize(7);
    m_range_data_maximum_rx1->setName("Maximum antenna 1");
    m_range_data_maximum_rx1->setPointLabelsVisible(true);
    m_range_data_maximum_rx1->setPointLabelsColor(Qt::white);
    m_range_data_maximum_rx1->setPointLabelsFormat("@xPoint m");

    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeBlueCerulean);
    chart->addSeries(m_range_data_series_rx1);
    chart->addSeries(m_range_data_series_rx2);
    chart->addSeries(m_range_data_maximum_rx1);

    chart->setTitle("Range spectrum");
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).back()->setRange(0, 10.0);
    chart->axes(Qt::Vertical).back()->setRange(0, 1.0);
    chart->axes(Qt::Horizontal).back()->setTitleText("Range [m]");
    chart->axes(Qt::Vertical).back()->setTitleText("Magnitude");

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

    QPointF max1(0,0);
    QPointF max2(0,0);
    for (size_t i = 0; i < rx1.size(); i++)
    {
        if (rx1[i].y() > max1.y())
        {
            max1 = rx1[i];
        }
        if (rx2[i].y() > max2.y())
        {
            max2 = rx2[i];
        }
    }

    m_range_data_maximum_rx1->clear();
    m_range_data_maximum_rx1->append(max1);
    m_range_data_maximum_rx1->append(max2);
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

