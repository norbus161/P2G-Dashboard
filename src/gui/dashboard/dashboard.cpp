#include "dashboard.h"
#include "./ui_dashboard.h"

#include <QObject>

Dashboard::Dashboard(QWidget *parent): QMainWindow(parent), ui(new Ui::Dashboard)
{
    ui->setupUi(this);
    m_statusbar = nullptr;
    m_toolbar = nullptr;
    m_settings = nullptr;
    resize(QDesktopWidget().availableGeometry(this).size() * 0.75);
}

Dashboard::~Dashboard()
{
    delete ui;
}

void Dashboard::setStatusbar(StatusBar *statusbar)
{
    if (statusbar == nullptr)
        return;

    m_statusbar = statusbar;
    setStatusBar(m_statusbar);
}

void Dashboard::setToolbar(ToolBar *toolbar)
{
    if (toolbar == nullptr)
        return;

    m_toolbar = toolbar;
    addToolBar(Qt::ToolBarArea::LeftToolBarArea, m_toolbar);
}

void Dashboard::setSettings(Settings *settings)
{
    if (settings == nullptr)
        return;

    m_settings = settings;
}

void Dashboard::setChart(QChart *chart, ChartType_t type)
{
    if (chart == nullptr)
        return;

    QChartView * v = nullptr;
    switch(type)
    {
        case ChartType_t::TimeData:
            v = ui->time_data;
            break;
        case ChartType_t::RangeData:
            v = ui->range_data;
            break;
        case ChartType_t::TargetData:
            v = ui->target_data;
            break;
    }

    v->setChart(chart);
    v->setRenderHint(QPainter::Antialiasing);
    v->chart()->layout()->setContentsMargins(0, 0, 0, 0);
    v->chart()->setBackgroundRoundness(0);
    v->show();
}

#ifdef _WIN32
void Dashboard::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
}
#endif
