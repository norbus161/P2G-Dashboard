#include "targetdataplot.h"

TargetDataPlot::TargetDataPlot()
{
    m_target_data_series = new QScatterSeries();
    initialize();
}

void TargetDataPlot::update(const Targets_t &data)
{
    m_target_data_series->clear();

    for (auto & e: data)
        m_target_data_series->append(QPointF(e.azimuth, e.radius / 100));
}

void TargetDataPlot::initialize()
{

    m_target_data_series->setName("Detected targets");
    m_target_data_series->setMarkerSize(12);
    m_target_data_series->setColor(Qt::red);
    m_target_data_series->setBorderColor(Qt::white);

    setTheme(QChart::ChartThemeBlueCerulean);
    addSeries(m_target_data_series);
    setTitle("Polar plot of targets");

    auto angularAxis = new QValueAxis();
    angularAxis->setRange(-180, 180);
    angularAxis->setTickAnchor(180);
    angularAxis->setTickCount(9);
    angularAxis->setLabelFormat("%d" + QString::fromLatin1("°"));
    addAxis(angularAxis, QPolarChart::PolarOrientationAngular);
    m_target_data_series->attachAxis(angularAxis);

    auto radialAxis = new QValueAxis();
    radialAxis->setRange(0, 10);
    radialAxis->setTickCount(6);
    radialAxis->setLabelFormat("%d [m]");
    addAxis(radialAxis, QPolarChart::PolarOrientationRadial);
    m_target_data_series->attachAxis(radialAxis);

    QFont font;
    font.setPixelSize(20);
    setTitleFont(font);
}
