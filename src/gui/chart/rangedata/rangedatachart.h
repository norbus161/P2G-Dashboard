#ifndef RANGEDATACHART_H
#define RANGEDATACHART_H

#include <misc/types.h>

#include <QtCharts>

using namespace QtCharts;

class RangeDataChart : public QChart
{
    Q_OBJECT

public:
    RangeDataChart();

public slots:
    void update(DataPoints_t const & rx1, DataPoints_t const & rx2,
                DataPoints_t const & maxima, double const & max_y);
    void setChartTheme(ChartTheme theme);
private:
    void initialize();
    void setFontSize(size_t size);

private:
    QAreaSeries m_range_data_series_rx1;
    QLineSeries m_range_data_series_upper_rx1;
    QAreaSeries m_range_data_series_rx2;
    QLineSeries m_range_data_series_upper_rx2;
    QScatterSeries m_range_data_maximum_rx1;
};

#endif // RANGEDATACHART_H
