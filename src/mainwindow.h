#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineSeries>
#include <QAreaSeries>
#include <QScatterSeries>

#include <EndpointRadarBase.h>
#include <EndpointTargetDetection.h>


QT_BEGIN_NAMESPACE
namespace QtCharts { class QLineSeries; }
namespace QtCharts { class QAreaSeries; }
namespace QtCharts { class QScatterSeries; }
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();    

    virtual void closeEvent(QCloseEvent *event) override;
    void initializeTimeDataPlot();
    void initializeRangeDataPlot();
    void initializeTargetDataPlot();

public slots:
    void updateTimeData(QList<QPointF> const & re_rx1, QList<QPointF> const & im_rx1,
                         QList<QPointF> const & re_rx2, QList<QPointF> const & im_rx2);
    void updateRangeData(QList<QPointF> const & data);
    void updateTargetData(QVector<Target_Info_t> const & data);

signals:
    void closed();

private:
    Ui::MainWindow *ui;
    QtCharts::QLineSeries * m_time_data_series_re_rx1;
    QtCharts::QLineSeries * m_time_data_series_im_rx1;
    QtCharts::QLineSeries * m_time_data_series_re_rx2;
    QtCharts::QLineSeries * m_time_data_series_im_rx2;
    QtCharts::QAreaSeries * m_range_data_series;
    QtCharts::QLineSeries * m_range_data_series_upper;
    QtCharts::QScatterSeries * m_target_data_series;
};
#endif // MAINWINDOW_H
