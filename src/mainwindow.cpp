#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initializeTimeDomain();
    initializePolarPlot();
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

void MainWindow::initializeTimeDomain()
{
    m_time_series_re_rx1 = new QLineSeries();
    m_time_series_im_rx1 = new QLineSeries();
    m_time_series_re_rx1->setName("Inphase component antenna 1");
    m_time_series_im_rx1->setName("Quadratur component antenna 1");

    m_time_series_re_rx2 = new QLineSeries();
    m_time_series_im_rx2 = new QLineSeries();
    m_time_series_re_rx2->setName("Inphase component antenna 2");
    m_time_series_im_rx2->setName("Quadratur component antenna 2");

    QChart *chart = new QChart();
    chart->addSeries(m_time_series_re_rx1);
    chart->addSeries(m_time_series_im_rx1);
    chart->addSeries(m_time_series_re_rx2);
    chart->addSeries(m_time_series_im_rx2);

    chart->createDefaultAxes();
    chart->setTitle("Time domain");
    chart->axes(Qt::Horizontal).back()->setRange(0, 63);
    chart->axes(Qt::Vertical).back()->setRange(0, 1.2);
    chart->setTheme(QChart::ChartThemeBlueCerulean);

    ui->time_domain->setChart(chart);
    ui->time_domain->setRenderHint(QPainter::Antialiasing);
    ui->time_domain->show();
}

void MainWindow::initializePolarPlot()
{
    const qreal angularMin = -180;
    const qreal angularMax = 180;

    const qreal radialMin = 0;
    const qreal radialMax = 10;

    m_polar_plot_series = new QScatterSeries();
    m_polar_plot_series->setName("Detected targets");

    QPolarChart *chart = new QPolarChart();
    chart->addSeries(m_polar_plot_series);

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

    m_polar_plot_series->attachAxis(radialAxis);
    m_polar_plot_series->attachAxis(angularAxis);

    radialAxis->setRange(radialMin, radialMax);
    angularAxis->setRange(angularMin, angularMax);
    angularAxis->setTickAnchor(angularMax);

    chart->setTitle("Polar plot of targets");
    chart->setTheme(QChart::ChartThemeBlueCerulean);

    ui->polar_plot->setChart(chart);
    ui->polar_plot->setRenderHint(QPainter::Antialiasing);
    ui->polar_plot->show();
}

void MainWindow::updateFrameData(QList<QPointF> const & re_rx1, QList<QPointF> const & im_rx1,
                                 QList<QPointF> const & re_rx2, QList<QPointF> const & im_rx2)
{
    m_time_series_re_rx1->clear();
    m_time_series_im_rx1->clear();
    m_time_series_re_rx2->clear();
    m_time_series_im_rx2->clear();

    m_time_series_re_rx1->append(re_rx1);
    m_time_series_im_rx1->append(im_rx1);
    m_time_series_re_rx2->append(re_rx2);
    m_time_series_im_rx2->append(im_rx2);
}

void MainWindow::updateTargetData(const QVector<Target_Info_t> &data)
{
    m_polar_plot_series->clear();

    for (auto & e: data)
    {
        //qDebug() << e.radius/100 << " : " << e.azimuth;
        m_polar_plot_series->append(e.radius/100, e.azimuth);
    }
}

