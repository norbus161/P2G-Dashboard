#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineSeries>
#include <QAreaSeries>
#include <QScatterSeries>

#include "types.h"
#include <EndpointRadarBase.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

#ifdef _WIN32
    virtual void closeEvent(QCloseEvent *event) override;
#endif
    void initializeTimeDataPlot();
    void initializeRangeDataPlot();
    void initializeTargetDataPlot();

public slots:
    void updateTimeData(DataPoints_t const & re_rx1, DataPoints_t const & im_rx1,
                        DataPoints_t const & re_rx2, DataPoints_t const & im_rx2);
    void updateRangeData(DataPoints_t const & rx1, DataPoints_t const & rx2);
    void updateTargetData(Targets_t const & data);

signals:
    void closed();

private:
    void calculateRangeMaxima(DataPoints_t const & rx1);

private:
    Ui::MainWindow *ui;
    QtCharts::QLineSeries * m_time_data_series_re_rx1;
    QtCharts::QLineSeries * m_time_data_series_im_rx1;
    QtCharts::QLineSeries * m_time_data_series_re_rx2;
    QtCharts::QLineSeries * m_time_data_series_im_rx2;

    QtCharts::QAreaSeries * m_range_data_series_rx1;
    QtCharts::QLineSeries * m_range_data_series_upper_rx1;
    QtCharts::QAreaSeries * m_range_data_series_rx2;
    QtCharts::QLineSeries * m_range_data_series_upper_rx2;
    QtCharts::QScatterSeries * m_range_data_maximum_rx1;

    QtCharts::QScatterSeries * m_target_data_series;
};
#endif // MAINWINDOW_H
