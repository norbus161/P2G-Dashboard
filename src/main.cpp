#include "mainwindow.h"
#include "radar.h"

#include <QApplication>
#include <QThread>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Instantiate radar object and and try to connect to it
    Radar r;
    while (!r.connect())
        QThread::msleep(250);

    // Add the necassery endpoints
    qDebug() << "Adding base endpoint: " << r.addEndpoint(EndpointType::Base);
    qDebug() << "Adding target detection endpoint: " << r.addEndpoint(EndpointType::TargetDetection);

    // Move the radar object into another thread, so the gui thread won't block
    QThread* t = new QThread();
    r.moveToThread(t);

    // Signal slot connections
    qRegisterMetaType<QList<QPointF>>("QList<QPointF>");
    qRegisterMetaType<QVector<Target_Info_t>>("QVector<Target_Info_t>");
    QObject::connect(&r, &Radar::timeDataChanged, &w, &MainWindow::updateTimeData, Qt::AutoConnection);
    QObject::connect(&r, &Radar::rangeDataChanged, &w, &MainWindow::updateRangeData, Qt::AutoConnection);
    QObject::connect(&r, &Radar::targetDataChanged, &w, &MainWindow::updateTargetData, Qt::AutoConnection);
    QObject::connect(t, &QThread::started, &r, &Radar::doMeasurement, Qt::AutoConnection);
    QObject::connect(&w, &MainWindow::closed, &r, &Radar::disconnect, Qt::DirectConnection);

    // Start the thread
    t->start();

    // Gui thread execution continues...
    w.show();
    return a.exec();
}
