#ifndef SIGNALPROCESSOR_H
#define SIGNALPROCESSOR_H

#include "types.h"


class SignalProcessor
{
public:
    SignalProcessor();
    DataPoints_t calculateRangeData(DataPoints_t const & re, DataPoints_t const & im);

private:
    void initialize();
    void setMeanValuesOfSignal(DataPoints_t const & re, DataPoints_t const & im);
    void generateComplexSignal(DataPoints_t const & re, DataPoints_t const & im);
    void generateHannWindow();
    void generateRangeVector();
    void windowComplexSignal();
    void calculateFft();

private:
    double m_re_mean;
    double m_im_mean;
    DoubleVec_t m_window;
    DoubleVec_t m_range_vec;
    ComplexVec_t m_complex_vec;
};

#endif // SIGNALPROCESSOR_H
