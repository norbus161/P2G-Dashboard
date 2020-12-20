#include "signalprocessor.h"

#include <dj_fft.h>
#include <math.h>

constexpr auto SIGNAL_SAMPLE_SIZE = 64;
constexpr auto SIGNAL_WINDOW_SIZE = SIGNAL_SAMPLE_SIZE;
constexpr auto SIGNAL_ZERO_PADDING_FACTOR = 4;
constexpr auto SIGNAL_ZERO_PADDED_SIZE = SIGNAL_SAMPLE_SIZE * SIGNAL_ZERO_PADDING_FACTOR;
constexpr auto SIGNAL_ZEROES_AMOUNT = SIGNAL_ZERO_PADDED_SIZE - SIGNAL_SAMPLE_SIZE;
constexpr auto SIGNAL_SIZE_DISCARD_HALF = SIGNAL_ZERO_PADDED_SIZE / 2;

constexpr auto RADAR_SAMPLING_FREQUENCY = 213.34 * 1e3;
constexpr auto RADAR_RAMP_TIME_EFF = 300 * 1e-6;
constexpr auto RADAR_BANDWITH_EFF = 200 * 1e6;
constexpr auto SPEED_OF_LIGHT = 3 * 1e8;

constexpr auto RANGE_SPECTRUM_DT = 1 / RADAR_SAMPLING_FREQUENCY;
constexpr auto RANGE_SPECTRUM_T_FFT = RANGE_SPECTRUM_DT * SIGNAL_ZERO_PADDED_SIZE;
constexpr auto RANGE_SPECTRUM_DF = 1 / RANGE_SPECTRUM_T_FFT;

SignalProcessor::SignalProcessor()
{
    initialize();
    generateHannWindow();
    generateRangeVector();
}

DataPoints_t SignalProcessor::calculateRangeData(const DataPoints_t &re, const DataPoints_t &im)
{
    initialize();
    DataPoints_t res;

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

void SignalProcessor::setMeanValuesOfSignal(const DataPoints_t &re, const DataPoints_t &im)
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

void SignalProcessor::generateComplexSignal(const DataPoints_t &re, const DataPoints_t &im)
{
    for (auto i = 0; i < SIGNAL_SAMPLE_SIZE; i++)
    {
        double real = re[i].y() - m_re_mean;
        double imag = im[i].y() - m_im_mean;

        Complex_t c(real, imag);
        m_complex_vec.push_back(c);
    }
}

void SignalProcessor::generateHannWindow()
{
    for (auto i = 0; i < SIGNAL_WINDOW_SIZE; i++)
    {
        double val = 0.5 * (1 - cos(2 * M_PI * i/(SIGNAL_WINDOW_SIZE-1)));
        m_window.push_back(val);
    }
}

void SignalProcessor::generateRangeVector()
{
    for (auto i = 0.0; i <= (RADAR_SAMPLING_FREQUENCY/2 - RANGE_SPECTRUM_DF); i+=RANGE_SPECTRUM_DF)
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
    ComplexVec_t res;

    for (auto i = 0; i < SIGNAL_SAMPLE_SIZE; i++)
    {
        res.push_back(m_complex_vec[i] * m_window[i]);
    }

    m_complex_vec = res;
}

void SignalProcessor::calculateFft()
{
    for (auto i = 0; i < SIGNAL_ZEROES_AMOUNT; i++)
    {
        Complex_t c (0,0);
        m_complex_vec.push_back(c);
    }

    m_complex_vec = dj::fft1d(m_complex_vec, dj::fft_dir::DIR_BWD);
}
