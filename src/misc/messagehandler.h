#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QDebug>
#include <QTime>

static void messageHandler(QtMsgType type, QMessageLogContext const &, QString const & msg)
{
    switch (type)
    {
        case QtDebugMsg:
            fprintf(stdout, "%s [DEBUG]: %s\n", QTime::currentTime().toString().toStdString().c_str(), msg.toStdString().c_str());
            fflush(stdout);
            break;
        case QtInfoMsg:
            fprintf(stdout, "%s [INFO]: %s\n", QTime::currentTime().toString().toStdString().c_str(), msg.toStdString().c_str());
            fflush(stdout);
            break;
        case QtWarningMsg:
            fprintf(stderr, "%s [WARNING]: %s\n", QTime::currentTime().toString().toStdString().c_str(), msg.toStdString().c_str());
            fflush(stderr);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "%s [CRITICAL]: %s\n", QTime::currentTime().toString().toStdString().c_str(), msg.toStdString().c_str());
            fflush(stderr);
            break;
        case QtFatalMsg:
            fprintf(stderr, "%s [FATAL]: %s\n", QTime::currentTime().toString().toStdString().c_str(), msg.toStdString().c_str());
            fflush(stderr);
            break;
    }
}

#endif // MESSAGEHANDLER_H
