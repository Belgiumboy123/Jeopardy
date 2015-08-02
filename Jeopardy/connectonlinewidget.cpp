#include "connectonlinewidget.h"
#include "ui_connectonlinewidget.h"

#include "qtutility.h"

ConnectOnlineWidget::ConnectOnlineWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ConnectOnlineWidget)
{
    m_ui->setupUi(this);

    setAutoFillBackground(true);
    auto mainPalette = palette();
    mainPalette.setColor(backgroundRole(), QColor(CLUE_BLUE));
    mainPalette.setColor(foregroundRole(), Qt::white);
    setPalette(mainPalette);

    QFont boardFont( QtUtil::GetBasicBoardFont() );
    QFont controlFont( QtUtil::GetBasicClueFont() );
    controlFont.setPointSize(40);

    auto titleLabelPal = m_ui->titleLabel->palette();
    titleLabelPal.setColor(m_ui->titleLabel->foregroundRole(), BOARD_TEXT);
    m_ui->titleLabel->setPalette(titleLabelPal);
    boardFont.setPointSize(120);
    m_ui->titleLabel->setFont(boardFont);

    QFont backFont(controlFont);
    backFont.setPointSize(32);
    m_ui->backButton->setFont(backFont);
    connect( m_ui->backButton, &QPushButton::clicked, this, &ConnectOnlineWidget::BackToMenu);

    m_ui->connectButton->setFont(controlFont);
    connect( m_ui->connectButton, &QPushButton::clicked, this, &ConnectOnlineWidget::OnConnectButton);

    m_ui->portEdit->setValidator( new QIntValidator(1025, 65535, this));
    m_ui->portEdit->setFont(controlFont);
    m_ui->portLabel->setFont(controlFont);

    m_ui->hostnameLabel->setFont(controlFont);
    m_ui->hostnameEdit->setFont(controlFont);

    m_ui->resultLabel->setFont(controlFont);
    auto resultLabelPal = m_ui->resultLabel->palette();
    resultLabelPal.setColor(m_ui->resultLabel->foregroundRole(), Qt::red);
    m_ui->resultLabel->setPalette(resultLabelPal);
}

void
ConnectOnlineWidget::OnConnectButton()
{

}

ConnectOnlineWidget::~ConnectOnlineWidget() {}
