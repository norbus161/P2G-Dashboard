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
    auto attempts = 5;
    while (attempts > 0 && !r.connect())
    {
        QThread::msleep(250);
        attempts--;
    }

    if (attempts == 0)
    {
        qCritical() << "Aborted: Failed to connect to radar sensor. ";
        return -1;
    }


    // Add the necassery endpoints
    qDebug() << "Adding base endpoint: " << r.addEndpoint(EndpointType::Base);
    qDebug() << "Adding target detection endpoint: " << r.addEndpoint(EndpointType::TargetDetection);

    // Disable automatic frame trigger
    r.setAutomaticFrameTrigger(false, EndpointType::Base, 0);
    r.setAutomaticFrameTrigger(false, EndpointType::TargetDetection, 0);

    // Move the radar object into another thread, so the gui thread won't block
    QThread* t = new QThread();
    r.moveToThread(t);

    // Signal slot connections
    qRegisterMetaType<QList<QPointF>>("QList<QPointF>");
    qRegisterMetaType<QVector<Target_Info_t>>("QVector<Target_Info_t>");
    QObject::connect(&r, &Radar::timeDataChanged, &w, &MainWindow::updateTimeData);
    QObject::connect(&r, &Radar::rangeDataChanged, &w, &MainWindow::updateRangeData);
    QObject::connect(&r, &Radar::targetDataChanged, &w, &MainWindow::updateTargetData);
    QObject::connect(t, &QThread::started, &r, &Radar::doMeasurement);
    QObject::connect(&w, &MainWindow::closed, &r, &Radar::disconnect);

    // Start the thread
    t->start();

    // Main thread execution continues...
    w.show();
    return a.exec();
}
