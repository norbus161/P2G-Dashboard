#include "targetdatachart.h"

TargetDataChart::TargetDataChart()
{
    initialize();
}

void TargetDataChart::update(const Targets_t &data)
{
    m_target_data_series.clear();

    for (auto & e: data)
        m_target_data_series.append(QPointF(e.azimuth, e.radius/100));
}

void TargetDataChart::setChartTheme(QChart::ChartTheme theme)
{
    setTheme(theme);
    setFontSize(20);
    m_target_data_series.setColor(Qt::red);
}

void TargetDataChart::initialize()
{
    m_target_data_series.setName("Detected targets");
    m_target_data_series.setMarkerSize(12);
    m_target_data_series.setColor(Qt::red);
    m_target_data_series.setBorderColor(Qt::white);

    setTheme(QChart::ChartThemeBlueCerulean);
    addSeries(&m_target_data_series);
    setTitle("Polar plot of targets");

    auto angularAxis = new QValueAxis();
    angularAxis->setRange(-180, 180);
    angularAxis->setTickAnchor(180);
    angularAxis->setTickCount(13);
    angularAxis->setLabelFormat("%d" + QString::fromLatin1("°"));
    addAxis(angularAxis, QPolarChart::PolarOrientationAngular);
    m_target_data_series.attachAxis(angularAxis);

    auto radialAxis = new QValueAxis();
    radialAxis->setRange(0, 10);
    radialAxis->setTickCount(6);
    radialAxis->setLabelFormat("%d [m]");
    addAxis(radialAxis, QPolarChart::PolarOrientationRadial);
    m_target_data_series.attachAxis(radialAxis);

    setFontSize(20);

    QLineSeries *seriesleft = new QLineSeries();
    seriesleft->append(-55, 100);
    seriesleft->append(0,0);
    seriesleft->append(55, 100);

    QLineSeries *seriesright = new QLineSeries();
    seriesright->append(-125, 100);
    seriesright->append(-125, 100);
    seriesright->append(-180, 100);

    QAreaSeries * area1 = new QAreaSeries(seriesleft, seriesright);
    addSeries(area1);
    area1->setBorderColor(Qt::transparent);
    area1->setOpacity(0.5);
    area1->setBrush(QColor("#c0000000"));
    area1->attachAxis(angularAxis);
    area1->attachAxis(radialAxis);
    legend()->markers(area1)[0]->setVisible(false);
}

void TargetDataChart::setFontSize(size_t size)
{
    QFont font;
    font.setPixelSize(size);
    setTitleFont(font);
}
