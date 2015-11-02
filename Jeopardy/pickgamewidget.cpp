#include "pickgamewidget.h"
#include "ui_pickgamewidget.h"

PickGameWidget::PickGameWidget(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::PickGameWidget)
{
    m_ui->setupUi(this);

    auto font = m_ui->titleLabel->font();
    font.setPointSize(125);
    m_ui->titleLabel->setFont(font);

    auto buttonFont = m_ui->jeopardyButton->font();
    buttonFont.setPointSize(50);
    m_ui->jeopardyButton->setFont(buttonFont);
    m_ui->battleshipButton->setFont(buttonFont);

    connect( m_ui->jeopardyButton, &QPushButton::clicked, this, &PickGameWidget::StartJeopardy);
    connect( m_ui->battleshipButton, &QPushButton::clicked, this, &PickGameWidget::StartBattleShip);
}

PickGameWidget::~PickGameWidget()
{
}
