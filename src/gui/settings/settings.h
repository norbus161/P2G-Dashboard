#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QtCharts>

#include <Protocol.h>
#include <EndpointRadarBase.h>
#include <EndpointTargetDetection.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Settings; }
QT_END_NAMESPACE


class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

public slots:
    void requestAll();
    void responseFrameFormat(Frame_Format_t const & frame_format);
    void responseDspSettings(DSP_Settings_t const & dsp_settings);

private slots:
    void applyButtonHandler();

signals:
    void chartThemeChanged(QtCharts::QChart::ChartTheme theme);
    void frameFormatChanged(Frame_Format_t const & frame_format);
    void dspSettingsChanged(DSP_Settings_t const & dsp_settings);
    void requestFrameFormat();
    void requestDspSettings();

private:
    Ui::Settings *ui;
    Frame_Format_t m_frame_format;
    DSP_Settings_t m_dsp_settings;
};

#endif // SETTINGS_H
