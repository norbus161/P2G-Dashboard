#include "targetdatachart.h"

TargetDataChart::TargetDataChart()
{
    m_target_data_series = new QScatterSeries();
    initialize();
}

void TargetDataChart::update(const Targets_t &data)
{
    m_target_data_series->clear();

    for (auto & e: data)
        m_target_data_series->append(QPointF(e.azimuth, e.radius/100));
}

void TargetDataChart::setChartTheme(QChart::ChartTheme theme)
{
    setTheme(theme);
}

void TargetDataChart::initialize()
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
    angularAxis->setTickCount(13);
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

    QLineSeries *seriesleft1 = new QLineSeries();
    seriesleft1->append(0,0);
    seriesleft1->append(55, 100);

    QLineSeries *seriesright1 = new QLineSeries();
    seriesright1->append(0,0);
    seriesright1->append(-180, 100);

    QLineSeries *seriesleft2 = new QLineSeries();
    seriesleft2->append(0,0);
    seriesleft2->append(-55, 100);

    QLineSeries *seriesright2 = new QLineSeries();
    seriesright2->append(0,0);
    seriesright2->append(-180, 100);

    QAreaSeries * area1 = new QAreaSeries(seriesleft1, seriesright1);
    addSeries(area1);
    area1->setBorderColor(Qt::transparent);
    area1->setOpacity(0.5);
    area1->setBrush(QColor("#c0000000"));
    area1->attachAxis(angularAxis);
    area1->attachAxis(radialAxis);
    legend()->markers(area1)[0]->setVisible(false);

    QAreaSeries * area2 = new QAreaSeries(seriesleft2, seriesright2);
    addSeries(area2);
    area2->setBorderColor(Qt::transparent);
    area2->setOpacity(0.5);
    area2->setBrush(QColor("#c0000000"));
    area2->attachAxis(angularAxis);
    area2->attachAxis(radialAxis);
    legend()->markers(area2)[0]->setVisible(false);
}
