#ifndef TIMEDATACHART_H
#define TIMEDATACHART_H

#include <misc/types.h>

#include <QtCharts>

using namespace QtCharts;

class TimeDataChart : public QChart
{
    Q_OBJECT

public:
    TimeDataChart();

public slots:
    void update(DataPoints_t const & re_rx1, DataPoints_t const & im_rx1,
                DataPoints_t const & re_rx2, DataPoints_t const & im_rx2);
    void setChartTheme(ChartTheme theme);
private:
    void initialize();

private:
    QLineSeries * m_time_data_series_re_rx1;
    QLineSeries * m_time_data_series_im_rx1;
    QLineSeries * m_time_data_series_re_rx2;
    QLineSeries * m_time_data_series_im_rx2;
};

#endif // TIMEDATACHART_H
