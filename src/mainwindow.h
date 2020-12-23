#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "types.h"

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

    void setPlot(QtCharts::QChart *plot, PlotType_t type);

#ifdef _WIN32
    virtual void closeEvent(QCloseEvent *event) override;
#endif

signals:
    void closed();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
