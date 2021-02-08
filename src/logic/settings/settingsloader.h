#ifndef SETTINGSLOADER_H
#define SETTINGSLOADER_H

#include <QObject>

#include <EndpointTargetDetection.h>

struct Settings_t
{
    bool statusbar_enabled;
    bool toolbar_enabled;
    DSP_Settings_t dsp_settings;
};

class SettingsLoader : public QObject
{
    Q_OBJECT
public:
    explicit SettingsLoader(QObject *parent = nullptr);
    bool parseSettings(QString const & path, Settings_t & settings);

private:
    bool readFile(QString const & path, QJsonDocument & data);

};

#endif // SETTINGSLOADER_H
