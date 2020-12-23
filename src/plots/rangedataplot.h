#ifndef RANGEDATAPLOT_H
#define RANGEDATAPLOT_H

#include "src/types.h"

#include <QtCharts>

using namespace QtCharts;

class RangeDataPlot : public QChart
{
    Q_OBJECT

public:
    RangeDataPlot();

public slots:
    void update(DataPoints_t const & rx1, DataPoints_t const & rx2,
                DataPoints_t const & maxima, double const & max_y);
private:
    void initialize();

private:
    QAreaSeries * m_range_data_series_rx1;
    QLineSeries * m_range_data_series_upper_rx1;
    QAreaSeries * m_range_data_series_rx2;
    QLineSeries * m_range_data_series_upper_rx2;
    QScatterSeries * m_range_data_maximum_rx1;
};

#endif // RANGEDATAPLOT_H
