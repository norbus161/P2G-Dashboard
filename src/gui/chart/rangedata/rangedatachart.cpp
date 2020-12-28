#include "rangedatachart.h"

RangeDataChart::RangeDataChart()
{
    m_range_data_series_rx1.setUpperSeries(&m_range_data_series_upper_rx1);
    m_range_data_series_rx2.setUpperSeries(&m_range_data_series_upper_rx2);

    initialize();
}

void RangeDataChart::update(const DataPoints_t &rx1, const DataPoints_t &rx2, const DataPoints_t &maxima, const double &max_y)
{
    m_range_data_series_upper_rx1.clear();
    m_range_data_series_upper_rx2.clear();
    m_range_data_maximum_rx1.clear();

    m_range_data_series_upper_rx1.append(rx1);
    m_range_data_series_upper_rx2.append(rx2);
    m_range_data_maximum_rx1.append(maxima);

    static_cast<QValueAxis*>(axes(Qt::Vertical).back())->setMax(max_y + 0.2);
}

void RangeDataChart::setChartTheme(QChart::ChartTheme theme)
{
    setTheme(theme);
    setFontSize(20);
    m_range_data_maximum_rx1.setBrush(Qt::red);
}

void RangeDataChart::initialize()
{
    m_range_data_series_rx1.setName("Antenna 1");
    m_range_data_series_rx2.setName("Antenna 2");

    m_range_data_maximum_rx1.setName("All extrema");
    m_range_data_maximum_rx1.setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_range_data_maximum_rx1.setMarkerSize(12);
    m_range_data_maximum_rx1.setBrush(Qt::red);
    m_range_data_maximum_rx1.setBorderColor(Qt::white);
    m_range_data_maximum_rx1.setPointLabelsVisible(true);
    m_range_data_maximum_rx1.setPointLabelsColor(Qt::white);
    m_range_data_maximum_rx1.setPointLabelsFormat("@xPoint m");

    setTheme(QChart::ChartThemeBlueCerulean);
    addSeries(&m_range_data_series_rx1);
    addSeries(&m_range_data_series_rx2);
    addSeries(&m_range_data_maximum_rx1);
    setTitle("Range spectrum");

    auto axisX = new QValueAxis;
    axisX->setTitleText("Range [m]");
    axisX->setRange(0, 10);
    axisX->setLabelFormat("%.1f");
    addAxis(axisX, Qt::AlignBottom);
    m_range_data_series_rx1.attachAxis(axisX);
    m_range_data_series_rx2.attachAxis(axisX);
    m_range_data_maximum_rx1.attachAxis(axisX);

    auto axisY = new QValueAxis;
    axisY->setTitleText("Magnitude");
    axisY->setRange(0, 1.0);
    axisY->setLabelFormat("%.1f");
    addAxis(axisY, Qt::AlignLeft);
    m_range_data_series_rx1.attachAxis(axisY);
    m_range_data_series_rx2.attachAxis(axisY);
    m_range_data_maximum_rx1.attachAxis(axisY);

    setFontSize(20);
}

void RangeDataChart::setFontSize(size_t size)
{
    QFont font;
    font.setPixelSize(size);
    setTitleFont(font);
}
