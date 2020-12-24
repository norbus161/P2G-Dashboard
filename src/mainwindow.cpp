#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_status = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setStatus(Status *status)
{
    if (status == nullptr)
        return;

    m_status = status;
}

void MainWindow::setPlot(QChart *plot, PlotType_t type)
{
    if (plot == nullptr)
        return;

    QChartView * v = nullptr;
    switch(type)
    {
        case PlotType_t::TimeData:
            v = ui->time_data;
            break;
        case PlotType_t::RangeData:
            v = ui->range_data;
            break;
        case PlotType_t::TargetData:
            v = ui->target_data;
            break;
    }

    v->setChart(plot);
    v->setRenderHint(QPainter::Antialiasing);
    v->chart()->layout()->setContentsMargins(0, 0, 0, 0);
    v->chart()->setBackgroundRoundness(0);
    v->show();
}

void MainWindow::updateStatus()
{
    if (m_status == nullptr)
        return;

    ui->statusBar->showMessage(m_status->get());
}

#ifdef _WIN32
void MainWindow::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
}
#endif


