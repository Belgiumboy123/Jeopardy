#pragma once

#include <QWidget>

namespace Ui {
class BattleshipGameWidget;
}

class BattleshipGameWidget : public QWidget
{
    Q_OBJECT

public:
    BattleshipGameWidget(QWidget *parent = nullptr);
    virtual ~BattleshipGameWidget();

private:
    std::unique_ptr<Ui::BattleshipGameWidget> m_ui;
};

