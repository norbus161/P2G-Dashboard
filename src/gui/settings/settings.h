#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QtCharts>


QT_BEGIN_NAMESPACE
namespace Ui { class Settings; }
QT_END_NAMESPACE

class Settings : public QDialog
{
    Q_OBJECT

public:
    Settings(QWidget *parent = nullptr);
    ~Settings();

private slots:
    void applyHandler();

signals:
    void chartThemeChanged(QtCharts::QChart::ChartTheme theme);

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
