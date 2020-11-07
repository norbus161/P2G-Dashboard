#include "signalprocessor.h"

#include <dj_fft.h>

#include <QPointF>
#include <math.h>

constexpr size_t SIGNAL_SAMPLE_SIZE = 64;
constexpr size_t SIGNAL_WINDOW_SIZE = SIGNAL_SAMPLE_SIZE;
constexpr size_t SIGNAL_ZERO_PADDING_FACTOR = 4;
constexpr size_t SIGNAL_ZERO_PADDED_SIZE = SIGNAL_SAMPLE_SIZE * SIGNAL_ZERO_PADDING_FACTOR;
constexpr size_t SIGNAL_ZEROES_AMOUNT = SIGNAL_ZERO_PADDED_SIZE - SIGNAL_SAMPLE_SIZE;
constexpr size_t SIGNAL_SIZE_DISCARD_HALF = SIGNAL_ZERO_PADDED_SIZE / 2;

constexpr double RADAR_SAMPLING_FREQUENCY = 213.34 * 1e3;
constexpr double RADAR_RAMP_TIME_EFF = 300 * 1e-6;
constexpr double RADAR_BANDWITH_EFF = 200 * 1e6;
constexpr double SPEED_OF_LIGHT = 3 * 1e8;

constexpr double RANGE_SPECTRUM_DT = 1 / RADAR_SAMPLING_FREQUENCY;
constexpr double RANGE_SPECTRUM_T_FFT = RANGE_SPECTRUM_DT * SIGNAL_ZERO_PADDED_SIZE;
constexpr double RANGE_SPECTRUM_DF = 1 / RANGE_SPECTRUM_T_FFT;

SignalProcessor::SignalProcessor()
{
    initialize();
    generateHannWindow();
    generateRangeVector();
}

QList<QPointF> SignalProcessor::calculateRangeData(const QList<QPointF> &re, const QList<QPointF> &im)
{
    initialize();
    QList<QPointF> res;

    if (re.size() > 0 && re.size() == im.size())
    {
        setMeanValuesOfSignal(re, im);
        generateComplexSignal(re, im);
        windowComplexSignal();
        calculateFft();
    }

    for (size_t i = 0; i < SIGNAL_SIZE_DISCARD_HALF; i++)
    {
        QPointF p(m_range_vec[i], std::abs(m_complex_vec[i]));
        res.push_back(p);
    }

    return res;
}

void SignalProcessor::setMeanValuesOfSignal(const QList<QPointF> &re, const QList<QPointF> &im)
{
    double re_sum = 0.0;
    double im_sum = 0.0;

    for (size_t i = 0; i < SIGNAL_SAMPLE_SIZE; i++)
    {
        re_sum += re[i].y();
        im_sum += im[i].y();
    }

    m_re_mean = re_sum / SIGNAL_SAMPLE_SIZE;
    m_im_mean = im_sum / SIGNAL_SAMPLE_SIZE;
}

void SignalProcessor::generateComplexSignal(const QList<QPointF> &re, const QList<QPointF> &im)
{
    for (size_t i = 0; i < SIGNAL_SAMPLE_SIZE; i++)
    {
        double real = re[i].y() - m_re_mean;
        double imag = im[i].y() - m_im_mean;

        std::complex<double> c(real, imag);
        m_complex_vec.push_back(c);
    }
}

void SignalProcessor::generateHannWindow()
{
    for (size_t i = 0; i < SIGNAL_WINDOW_SIZE; i++)
    {
        double val = 0.5 * (1 - cos(2 * M_PI * i/(SIGNAL_WINDOW_SIZE-1)));
        m_window.push_back(val);
    }
}

void SignalProcessor::generateRangeVector()
{
    for (double i = 0.0; i <= (RADAR_SAMPLING_FREQUENCY/2 - RANGE_SPECTRUM_DF); i+=RANGE_SPECTRUM_DF)
    {
        auto val = i * RADAR_RAMP_TIME_EFF * SPEED_OF_LIGHT / (2 * RADAR_BANDWITH_EFF);
        m_range_vec.push_back(QString::number(val, 'f', 2).toDouble());
    }
}

void SignalProcessor::initialize()
{
    m_re_mean = 0.0;
    m_im_mean = 0.0;
    m_complex_vec.clear();
}

void SignalProcessor::windowComplexSignal()
{
    std::vector<std::complex<double>> res;

    for (size_t i = 0; i < SIGNAL_SAMPLE_SIZE; i++)
    {
        res.push_back(m_complex_vec[i] * m_window[i]);
    }

    m_complex_vec = res;
}

void SignalProcessor::calculateFft()
{
    for (size_t i = 0; i < SIGNAL_ZEROES_AMOUNT; i++)
    {
        std::complex<double> c (0,0);
        m_complex_vec.push_back(c);
    }

    m_complex_vec = dj::fft1d(m_complex_vec, dj::fft_dir::DIR_BWD);
}
