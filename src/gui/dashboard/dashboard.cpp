#include "dashboard.h"
#include "./ui_dashboard.h"

#include <QObject>

Dashboard::Dashboard(QWidget *parent): QMainWindow(parent), ui(new Ui::Dashboard)
{
    ui->setupUi(this);
    m_status = nullptr;
}

Dashboard::~Dashboard()
{
    delete ui;
}

void Dashboard::setStatus(Status *status)
{
    if (status == nullptr)
        return;

    m_status = status;
}

void Dashboard::setSettings(Settings *settings)
{
    if (settings == nullptr)
        return;

    m_settings = settings;
    m_settings->setModal(false);

    auto open_settings = new QAction("&Settings", this);
    auto s = menuBar()->addMenu("&View");
    s->addAction(open_settings);

    connect(open_settings, &QAction::triggered, this, [=]()
    {
        m_settings->show();
    });
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

void Dashboard::updateStatus()
{
    if (m_status == nullptr)
        return;

    ui->statusBar->showMessage(m_status->get());
}

#ifdef _WIN32
void Dashboard::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
}
#endif


