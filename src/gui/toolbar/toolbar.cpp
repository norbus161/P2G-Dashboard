#include "toolbar.h"

ToolBar::ToolBar(QWidget *parent) : QToolBar("Toolbar", parent)
{
    auto settings_icon = QPixmap(":/resources/icons/settings.ico");
    auto action_settings = addAction(QIcon(settings_icon), "Settings");

    QObject::connect(action_settings, &QAction::triggered, this, [=]()
    {
        emit settingsClicked();
    });

    setFloatable(false);
    setMovable(false);
}
