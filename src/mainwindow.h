#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "types.h"
#include "status.h"

#include <EndpointRadarBase.h>
#include <QMainWindow>
#include <QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setStatus(Status *status);
    void setPlot(QtCharts::QChart *plot, PlotType_t type);

public slots:
    void updateStatus();

#ifdef _WIN32
    virtual void closeEvent(QCloseEvent *event) override;
#endif

signals:
    void closed();

private:
    Ui::MainWindow *ui;
    Status * m_status;
};
#endif // MAINWINDOW_H
