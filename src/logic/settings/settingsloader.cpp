#include "settingsloader.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

SettingsLoader::SettingsLoader(QObject *parent) : QObject(parent)
{}

bool SettingsLoader::parseSettings(const QString &path, Settings_t &settings)
{
    QJsonDocument data;
    if (!readFile(path, data))
        return false;

    QJsonObject json = data.object();
    settings.statusbar_enabled = json["StatusbarEnabled"].toBool();
    settings.toolbar_enabled = json["ToolbarEnabled"].toBool();

    QJsonObject dsp = json.value("DspSettings").toObject();
    settings.dsp_settings.range_mvg_avg_length = dsp["RangeMovingAverageFilterLength"].toInt();
    settings.dsp_settings.min_range_cm = dsp["MinRange"].toInt();
    settings.dsp_settings.max_range_cm = dsp["MaxRange"].toInt();
    settings.dsp_settings.min_speed_kmh = dsp["MinSpeed"].toInt();
    settings.dsp_settings.max_speed_kmh = dsp["MaxSpeed"].toInt();
    settings.dsp_settings.speed_threshold = dsp["SpeedThreshold"].toInt();
    settings.dsp_settings.range_threshold = dsp["RangeThreshold"].toInt();
    settings.dsp_settings.enable_tracking = dsp["Tracking"].toInt();
    settings.dsp_settings.num_of_tracks = dsp["NumberOfTracks"].toInt();
    settings.dsp_settings.median_filter_length = dsp["MedianFilterDepth"].toInt();
    settings.dsp_settings.enable_mti_filter = dsp["MTIFilterSelection"].toInt();
    settings.dsp_settings.mti_filter_length = dsp["MTIFilterWeight"].toInt();

    return true;
}

bool SettingsLoader::readFile(const QString &path, QJsonDocument & data)
{
    QFile in(path);
    if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Error while opening settings file";
        return false;
    }

    QJsonParseError err;
    data = QJsonDocument::fromJson(in.readAll(), &err);
    in.close();

    if (err.error != QJsonParseError::NoError)
    {
        qWarning() << "Error while parsing json document: " << err.errorString();
        return false;
    }

    return true;
}
