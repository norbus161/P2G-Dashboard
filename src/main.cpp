#include "mainwindow.h"
#include "radar.h"

#include <QApplication>
#include <QThread>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Instantiate radar object and configure it
    Radar r;
    qDebug() << "Connecting: " << r.Connect();
    qDebug() << "Setting endpoint: " << r.AddEndpoint(EndpointType::Base);
    qDebug() << "Enable frame trigger: " << r.EnableAutomaticFrameTrigger(EndpointType::Base, 1000000);

    // Move the radar object into another thread, so the gui thread won't block
    QThread* t = new QThread();
    r.moveToThread(t);

    // Connections
    QObject::connect(t, SIGNAL(started()), &r, SLOT(DoMeasurement()));

    // Start the thread
    t->start();

    // Gui thread continues...
    w.show();
    return a.exec();
}
