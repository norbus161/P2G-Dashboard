#ifndef TARGETDATAPLOT_H
#define TARGETDATAPLOT_H

#include "src/types.h"

#include <QtCharts>

using namespace QtCharts;

class TargetDataPlot : public QPolarChart
{
    Q_OBJECT

public:
    TargetDataPlot();

public slots:
    void update(Targets_t const & data);

private:
    void initialize();

private:
    QScatterSeries * m_target_data_series;
};

#endif // TARGETDATAPLOT_H
