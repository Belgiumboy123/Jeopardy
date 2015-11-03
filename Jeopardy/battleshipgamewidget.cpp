#include "battleshipgamewidget.h"
#include "ui_battleshipgamewidget.h"

BattleshipGameWidget::BattleshipGameWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::BattleshipGameWidget)
{
    m_ui->setupUi(this);
}

BattleshipGameWidget::~BattleshipGameWidget()
{
}
