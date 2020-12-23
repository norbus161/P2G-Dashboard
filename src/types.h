#ifndef TYPES_H
#define TYPES_H

#include <EndpointTargetDetection.h>
#include <QList>
#include <QVector>
#include <QPointF>
#include <complex>

using DataPoints_t = QList<QPointF>;
using Targets_t = QVector<Target_Info_t>;

using DoubleVec_t = std::vector<double>;
using Complex_t = std::complex<double>;
using ComplexVec_t = std::vector<Complex_t>;


enum class PlotType_t
{
    TimeData,
    RangeData,
    TargetData
};

enum class EndpointType_t
{
    Calibration,
    AdcXmc,
    Base,
    Doppler,
    Fmcw,
    Industrial,
    P2G,
    TargetDetection
};

#endif // TYPES_H
