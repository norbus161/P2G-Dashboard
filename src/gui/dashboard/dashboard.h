#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <misc/types.h>
#include <gui/settings/settings.h>
#include <gui/statusbar/statusbar.h>
#include <gui/toolbar/toolbar.h>

#include <EndpointRadarBase.h>
#include <QMainWindow>
#include <QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui { class Dashboard; }
QT_END_NAMESPACE


class Dashboard : public QMainWindow
{
    Q_OBJECT

public:
    Dashboard(QWidget *parent = nullptr);
    ~Dashboard();

    void setStatusbar(StatusBar *statusbar);
    void setToolbar(ToolBar *toolbar);
    void setSettings(Settings *settings);
    void setChart(QtCharts::QChart *chart, ChartType_t type);

public slots:
#ifdef _WIN32
    virtual void closeEvent(QCloseEvent *event) override;
#endif

signals:
    void closed();

private:
    Ui::Dashboard *ui;
    StatusBar *m_statusbar;
    ToolBar *m_toolbar;
    Settings *m_settings;
};
#endif // DASHBOARD_H
