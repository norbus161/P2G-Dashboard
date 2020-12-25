#include "settings.h"
#include "./ui_settings.h"

#include <QDebug>

Settings::Settings(QWidget *parent): QDialog(parent), ui(new Ui::Settings)
{
    ui->setupUi(this);
    QObject::connect(ui->pushButton, &QPushButton::clicked, this, &Settings::applyHandler);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::applyHandler()
{
    qDebug() << "Hi";
    emit chartThemeChanged(static_cast<QtCharts::QChart::ChartTheme>(ui->spinBox->value()));
}
