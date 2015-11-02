#ifndef PICKGAMEWIDGET_H
#define PICKGAMEWIDGET_H

#include <QWidget>

namespace Ui {
class PickGameWidget;
}

class PickGameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PickGameWidget(QWidget *parent = 0);
    virtual ~PickGameWidget();

signals:
    void StartJeopardy();
    void StartBattleShip();

private:
    std::unique_ptr<Ui::PickGameWidget> m_ui;
};

#endif // PICKGAMEWIDGET_H
