#include "connectonlinewidget.h"
#include "ui_connectonlinewidget.h"

#include "qtutility.h"
#include "statehandleronline.h"

#include <QDebug>

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

    m_ui->buttonsWidget->setFixedSize(m_ui->buttonsWidget->sizeHint());

    m_ui->portEdit->setValidator(new QIntValidator(1025, 65535, this));
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
ConnectOnlineWidget::BeginConnection(std::unique_ptr<StateHandlerOnline> stateHandler)
{
    m_stateHandler = std::move(stateHandler);
    connect( m_stateHandler.get(), &StateHandlerOnline::ConnectionMade, this, &ConnectOnlineWidget::OnConnectionMade);
    connect( m_stateHandler.get(), &StateHandlerOnline::ConnectionLost, this, &ConnectOnlineWidget::OnConnectionLost);
    connect( m_stateHandler.get(), &StateHandlerOnline::ConnectionMessage, this, &ConnectOnlineWidget::OnConnectionMessage);


    m_ui->resultLabel->setText("");
    ShowState(ENTRY);
}

void
ConnectOnlineWidget::OnConnectionMade()
{
    ShowState(CONNECTED);
}

void
ConnectOnlineWidget::OnConnectionLost(const QString& message)
{
    m_ui->resultLabel->setText(message);
    ShowState(ENTRY);
}

void
ConnectOnlineWidget::OnConnectionMessage(const QString& message)
{
    if( m_state == ENTRY )
    {
        m_ui->resultLabel->setText(message);
    }
    else if( m_state == CONNECTED)
    {
        m_ui->resultLabel->setText(tr("two players connected! messeage from server: %1").arg(message));
    }
}

void
ConnectOnlineWidget::OnConnectButton()
{
    const QString& hostname = m_ui->hostnameEdit->text();
    bool isInt;
    const int portNumber = m_ui->portEdit->text().toInt(&isInt);

    if( hostname.isEmpty() || !isInt)
    {
        m_ui->resultLabel->setText("Oops, server name or port number is an in incorrect format.");
    }
    else
    {
        m_stateHandler->ConnectToHost(hostname, portNumber);
        m_ui->resultLabel->setText("Attempting to connect to server...");
        m_ui->connectButton->hide();
    }
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
        // if ok send message to state handler to disconnect
        // and show entry -> ShowState(ENTRY);
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
