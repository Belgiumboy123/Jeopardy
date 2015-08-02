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

    QFont headerFont(controlFont);
    headerFont.setPointSize(52);
    m_ui->headerLabel->setFont(headerFont);

    QFont backFont(controlFont);
    backFont.setPointSize(32);
    m_ui->backButton->setFont(backFont);
    connect( m_ui->backButton, &QPushButton::clicked, this, &ConnectOnlineWidget::OnBackButton);

    m_ui->connectButton->setFont(controlFont);
    connect( m_ui->connectButton, &QPushButton::clicked, this, &ConnectOnlineWidget::OnConnectButton);

    m_ui->startGameButton->setFont(controlFont);
    connect(m_ui->startGameButton, &QPushButton::clicked, this, &ConnectOnlineWidget::OnStartGameButton);

    m_ui->portEdit->setValidator(new QIntValidator(1025, 65535, this));
    m_ui->portEdit->setFont(controlFont);
    m_ui->portLabel->setFont(controlFont);

    m_ui->hostnameLabel->setFont(controlFont);
    m_ui->hostnameEdit->setFont(controlFont);

    m_ui->resultLabel->setFont(controlFont);
    auto resultLabelPal = m_ui->resultLabel->palette();
    resultLabelPal.setColor(m_ui->resultLabel->foregroundRole(), Qt::red);
    m_ui->resultLabel->setPalette(resultLabelPal);

    ShowState(ENTRY);
}

void
ConnectOnlineWidget::OnConnectButton()
{
    ShowState(START);
}

void
ConnectOnlineWidget::OnBackButton()
{
    switch( m_state)
    {
    case ENTRY:
        emit BackToMenu();
        break;

    case START:
    case CONNECTED:
        // TODO show warning dialog - leave server
        ShowState(ENTRY);
        break;

    default:
        break;
    }
}

void
ConnectOnlineWidget::OnStartGameButton()
{
    // todo send message to server -> start game has been clicked.
    // On server start we send out the GameStart signal

    m_ui->startGameButton->hide();
    m_ui->headerLabel->setText("Starting game!");
    m_ui->resultLabel->setText("Waiting for other player...");
    m_ui->resultLabel->show();
}

void
ConnectOnlineWidget::ShowState( State state)
{
    m_state = state;

    switch( m_state )
    {
    case ENTRY:
        m_ui->headerLabel->hide();
        m_ui->startGameButton->hide();
        m_ui->portWidget->show();
        m_ui->hostnameWidget->show();
        m_ui->connectButton->show();
        m_ui->resultLabel->setText("");
        m_ui->resultLabel->show();
        break;

    case CONNECTED:
        m_ui->portWidget->hide();
        m_ui->hostnameWidget->hide();
        m_ui->connectButton->hide();
        m_ui->startGameButton->hide();
        m_ui->resultLabel->setText("Waiting for another player to join...");
        m_ui->resultLabel->show();
        m_ui->headerLabel->setText("Connected to Server!");
        m_ui->headerLabel->show();
        break;

    case START:
        m_ui->portWidget->hide();
        m_ui->hostnameWidget->hide();
        m_ui->connectButton->hide();
        m_ui->resultLabel->hide();
        m_ui->headerLabel->setText("Both players are connected!");
        m_ui->headerLabel->show();
        m_ui->startGameButton->show();
        break;

    default:
        break;
    }
}

ConnectOnlineWidget::~ConnectOnlineWidget() {}
