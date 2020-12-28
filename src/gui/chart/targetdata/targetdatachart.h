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

private:
    void initialize();
    void setFontSize(size_t size);

private:
    QScatterSeries m_target_data_series;
};

#endif // TARGETDATACHART_H
