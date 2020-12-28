#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit ToolBar(QWidget *parent = nullptr);

signals:
    void settingsClicked();
};

#endif // TOOLBAR_H
