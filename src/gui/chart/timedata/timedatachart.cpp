#include "timedatachart.h"

#include <QFont>

TimeDataChart::TimeDataChart()
{
    initialize();
}

void TimeDataChart::update(const DataPoints_t &re_rx1, const DataPoints_t &im_rx1, const DataPoints_t &re_rx2, const DataPoints_t &im_rx2)
{
    m_time_data_series_re_rx1.clear();
    m_time_data_series_im_rx1.clear();
    m_time_data_series_re_rx2.clear();
    m_time_data_series_im_rx2.clear();

    m_time_data_series_re_rx1.append(re_rx1);
    m_time_data_series_im_rx1.append(im_rx1);
    m_time_data_series_re_rx2.append(re_rx2);
    m_time_data_series_im_rx2.append(im_rx2);
}

void TimeDataChart::initialize()
{

    m_time_data_series_re_rx1.setName("Inphase signal<br>component antenna 1");
    m_time_data_series_im_rx1.setName("Quadratur signal<br>component antenna 1");
    m_time_data_series_re_rx2.setName("Inphase signal<br>component antenna 2");
    m_time_data_series_im_rx2.setName("Quadratur signal<br>component antenna 2");

    setTheme(QChart::ChartThemeBlueCerulean);
    setTitle("Time domain");
    addSeries(&m_time_data_series_re_rx1);
    addSeries(&m_time_data_series_im_rx1);
    addSeries(&m_time_data_series_re_rx2);
    addSeries(&m_time_data_series_im_rx2);

    auto axisX = new QValueAxis();
    axisX->setTitleText("Samples");
    axisX->setRange(0, 63);
    axisX->setLabelFormat("%d");
    addAxis(axisX, Qt::AlignBottom);
    m_time_data_series_re_rx1.attachAxis(axisX);
    m_time_data_series_im_rx1.attachAxis(axisX);
    m_time_data_series_re_rx2.attachAxis(axisX);
    m_time_data_series_im_rx2.attachAxis(axisX);

    auto axisY = new QValueAxis();
    axisY->setTitleText("Amplitude");
    axisY->setRange(0, 1.2);
    axisY->setLabelFormat("%.1f");
    addAxis(axisY, Qt::AlignLeft);
    m_time_data_series_re_rx1.attachAxis(axisY);
    m_time_data_series_im_rx1.attachAxis(axisY);
    m_time_data_series_re_rx2.attachAxis(axisY);
    m_time_data_series_im_rx2.attachAxis(axisY);

    setFontSize(20);
}

void TimeDataChart::setFontSize(size_t size)
{
    QFont font;
    font.setPixelSize(size);
    setTitleFont(font);
}
