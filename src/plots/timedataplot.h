#ifndef TIMEDATAPLOT_H
#define TIMEDATAPLOT_H

#include "src/types.h"

#include <QtCharts>

using namespace QtCharts;

class TimeDataPlot : public QChart
{
    Q_OBJECT

public:
    TimeDataPlot();

public slots:
    void update(DataPoints_t const & re_rx1, DataPoints_t const & im_rx1,
                DataPoints_t const & re_rx2, DataPoints_t const & im_rx2);
private:
    void initialize();

private:
    QLineSeries * m_time_data_series_re_rx1;
    QLineSeries * m_time_data_series_im_rx1;
    QLineSeries * m_time_data_series_re_rx2;
    QLineSeries * m_time_data_series_im_rx2;
};

#endif // TIMEDATAPLOT_H
