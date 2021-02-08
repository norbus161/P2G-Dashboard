#include "settings.h"
#include "./ui_settings.h"

#include <QDebug>

Settings::Settings(QWidget *parent): QDialog(parent), ui(new Ui::Settings)
{
    ui->setupUi(this);
    setModal(false);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QObject::connect(ui->btn_apply, &QPushButton::clicked, this, &Settings::applyButtonHandler);
    QObject::connect(ui->btn_cancel, &QPushButton::clicked, this, &Settings::close);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::requestAll()
{
    emit requestFrameFormat();
    emit requestDspSettings();
}

void Settings::applyButtonHandler()
{
    auto copy_frame_format = m_frame_format;
    copy_frame_format.num_chirps_per_frame = ui->txt_chirps_per_frame->text().toInt();
    copy_frame_format.num_samples_per_chirp = ui->txt_samples_per_chirp->text().toInt();
    emit frameFormatChanged(copy_frame_format);

    auto copy_dsp_settings = m_dsp_settings;
    copy_dsp_settings.range_mvg_avg_length = ui->txt_range_moving_average->text().toInt();
    copy_dsp_settings.min_range_cm = ui->txt_min_range->text().toInt();
    copy_dsp_settings.max_range_cm = ui->txt_max_range->text().toInt();
    copy_dsp_settings.min_speed_kmh = ui->txt_min_speed->text().toInt();
    copy_dsp_settings.max_speed_kmh = ui->txt_max_speed->text().toInt();
    copy_dsp_settings.speed_threshold = ui->txt_speed_threshold->text().toInt();
    copy_dsp_settings.range_threshold = ui->txt_range_threshold->text().toInt();
    copy_dsp_settings.enable_tracking = ui->check_tracking->isChecked();
    copy_dsp_settings.num_of_tracks = ui->txt_number_of_track->text().toInt();
    copy_dsp_settings.median_filter_length = ui->txt_median_filter_depth->text().toInt();
    copy_dsp_settings.enable_mti_filter = ui->check_mti_filter_selection->isChecked();
    copy_dsp_settings.mti_filter_length = ui->txt_mti_filter_weight->text().toInt();    
    emit dspSettingsChanged(copy_dsp_settings);
    emit requestDspSettings(); // Read back immediately (just necassery for dsp settings)
}

void Settings::responseFrameFormat(const Frame_Format_t &frame_format)
{
    m_frame_format = frame_format;
    ui->txt_chirps_per_frame->setText(QString::number(m_frame_format.num_chirps_per_frame));
    ui->txt_samples_per_chirp->setText(QString::number(m_frame_format.num_samples_per_chirp));
}

void Settings::responseDspSettings(const DSP_Settings_t &dsp_settings)
{
    m_dsp_settings = dsp_settings;
    ui->txt_range_moving_average->setText(QString::number(dsp_settings.range_mvg_avg_length));
    ui->txt_min_range->setText(QString::number(dsp_settings.min_range_cm));
    ui->txt_max_range->setText(QString::number(dsp_settings.max_range_cm));
    ui->txt_min_speed->setText(QString::number(dsp_settings.min_speed_kmh));
    ui->txt_max_speed->setText(QString::number(dsp_settings.max_speed_kmh));
    ui->txt_speed_threshold->setText(QString::number(dsp_settings.speed_threshold));
    ui->txt_range_threshold->setText(QString::number(dsp_settings.range_threshold));
    ui->check_tracking->setChecked(dsp_settings.enable_tracking);
    ui->txt_number_of_track->setText(QString::number(dsp_settings.num_of_tracks));
    ui->txt_median_filter_depth->setText(QString::number(dsp_settings.median_filter_length));
    ui->check_mti_filter_selection->setChecked(dsp_settings.enable_mti_filter);
    ui->txt_mti_filter_weight->setText(QString::number(dsp_settings.mti_filter_length));
}
