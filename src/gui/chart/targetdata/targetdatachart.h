#ifndef TARGETDATACHART_H
#define TARGETDATACHART_H

#include <misc/types.h>

#include <QtCharts>

using namespace QtCharts;

class TargetDataChart : public QPolarChart
{
    Q_OBJECT

public:
    TargetDataChart();

public slots:
    void update(Targets_t const & data);
    void setChartTheme(ChartTheme theme);

private:
    void initialize();

private:
    QScatterSeries * m_target_data_series;
};

#endif // TARGETDATACHART_H
