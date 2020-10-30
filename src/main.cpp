#include "mainwindow.h"
#include "radar.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    Radar r;
    qDebug() << "Connecting: " << r.Connect();
    qDebug() << "Setting endpoint: " << r.AddEndpoint(EndpointType::Base);
    qDebug() << "Enable frame trigger: " << r.EnableAutomaticFrameTrigger(EndpointType::Base, 1000000);

    w.show();
    return a.exec();
}
