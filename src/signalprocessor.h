#ifndef SIGNALPROCESSOR_H
#define SIGNALPROCESSOR_H

#include <QList>
#include <QPointF>
#include <complex>

class SignalProcessor
{
public:
    SignalProcessor();
    QList<QPointF> calculateRangeData(QList<QPointF> const & re, QList<QPointF> const & im);

private:
    void initialize();
    void setMeanValuesOfSignal(QList<QPointF> const & re, QList<QPointF> const & im);
    void generateComplexSignal(QList<QPointF> const & re, QList<QPointF> const & im);
    void generateHannWindow();
    void generateRangeVector();
    void windowComplexSignal();
    void calculateFft();

private:
    double m_re_mean;
    double m_im_mean;
    std::vector<double> m_window;
    std::vector<double> m_range_vec;
    std::vector<std::complex<double>> m_complex_vec;
};

#endif // SIGNALPROCESSOR_H
