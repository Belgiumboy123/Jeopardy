#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "battleshipstatehandleronline.h"
#include "optionsdialog.h"
#include "pausedialog.h"
#include "qtutility.h"
#include "statehandleroffline.h"
#include "jeopardystatehandleronline.h"

#include <QFontDatabase>
#include <QKeyEvent>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent/*=nullptr*/)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    // Load the two needed fonts from resources.qrc
    QFontDatabase::addApplicationFont(":/korinna");
    QFontDatabase::addApplicationFont(":/swiss");
    QFontDatabase::addApplicationFont(":/swiss2");
    QFontDatabase::addApplicationFont(":/trium");

    // the toolbar is unused for now
    m_ui->mainToolBar->hide();

    QFont titleFont( QtUtil::GetBasicTitleFont() );
    QFont clueFont( QtUtil::GetBasicClueFont() );

    auto jeopardyTitleLabelPal = m_ui->jeopardyTitleLabel->palette();
    jeopardyTitleLabelPal.setColor(m_ui->jeopardyTitleLabel->foregroundRole(), BOARD_TEXT);
    m_ui->jeopardyTitleLabel->setPalette(jeopardyTitleLabelPal);
    titleFont.setPointSize(200);
    m_ui->jeopardyTitleLabel->setFont(titleFont);

    QFont buttonFont(clueFont);
    buttonFont.setPointSize(40);
    m_ui->startGameButton->setFont(buttonFont);
    connect( m_ui->startGameButton, &QPushButton::clicked, this, &MainWindow::OnOfflineGameStart );

    QFont backFont(buttonFont);
    backFont.setPointSize(32);
    m_ui->backButton->setFont(backFont);
    connect( m_ui->backButton, &QPushButton::clicked, this, &MainWindow::OnBack);
    m_ui->backWidget->setFixedSize(m_ui->backWidget->sizeHint());

    m_ui->pickJeopardyModeWidget->setAutoFillBackground(true);
    auto pickGamePal = m_ui->pickJeopardyModeWidget->palette();
    pickGamePal.setColor(m_ui->pickJeopardyModeWidget->backgroundRole(), QColor(CLUE_BLUE));
    pickGamePal.setColor(m_ui->pickJeopardyModeWidget->foregroundRole(), Qt::white);
    m_ui->pickJeopardyModeWidget->setPalette(pickGamePal);

    clueFont.setPointSize(28);
    m_ui->autoPlayCheckBox->setFont(clueFont);

    clueFont.setPointSize(20);
    m_ui->optionsButton->setFont(clueFont);
    connect( m_ui->optionsButton, &QPushButton::clicked, this, &MainWindow::launchOptionsDialog );

    // make sure everything is packed together nicely
    m_ui->optionsWidget->setFixedSize(m_ui->optionsWidget->sizeHint());

    connect( m_ui->pickGameWidget, &PickGameWidget::StartJeopardy, this, &MainWindow::OnJeopardyStart);
    connect( m_ui->pickGameWidget, &PickGameWidget::StartBattleShip, this, &MainWindow::OnBattleShipStart);

    // Lets us know when a game is over
    connect( m_ui->jeopardyGameWidget, &JeopardyGameWidget::GameOver, this, &MainWindow::OnGameOver);
    m_ui->jeopardyGameWidget->SetOptions(OptionsData::GetInstance());

    connect( m_ui->connectOnlineWidget, &ConnectOnlineWidget::BackToMenu, this, &MainWindow::OnBack);
    connect( m_ui->connectOnlineWidget, &ConnectOnlineWidget::StartGame, this, &MainWindow::OnOnlineGameStart);

    connect( m_ui->autoPlayCheckBox, &QCheckBox::toggled, m_ui->jeopardyGameWidget, &JeopardyGameWidget::SetAutoPlayEnabled);

    m_ui->onlineButton->setFont(buttonFont);
    connect(  m_ui->onlineButton, &QPushButton::clicked, this, &MainWindow::OnJeoOnlineButtonClicked);

    m_ui->offlineButton->setFont(buttonFont);
    connect( m_ui->offlineButton, &QPushButton::clicked, this, &MainWindow::OnJeoOfflineButtonClicked);

    ShowGameState(PICK_GAME);
    showMaximized();
}

void
MainWindow::OnJeopardyStart()
{
    ShowGameState(PICK_JEO_OFFLINE_ONLINE);
}

void
MainWindow::OnBattleShipStart()
{
    std::unique_ptr<IStateHandlerOnline> stateHandler(new BattleshipStateHandlerOnline);
    m_ui->connectOnlineWidget->BeginConnection(std::move(stateHandler));
    ShowGameState(ONLINE_BS_MENU);
}

void
MainWindow::OnGameOver()
{
    ShowGameState(PICK_JEO_OFFLINE_ONLINE);
}

void
MainWindow::OnJeoOnlineButtonClicked()
{
    std::unique_ptr<IStateHandlerOnline> stateHandler(new JeopardyStateHandlerOnline);
    m_ui->connectOnlineWidget->BeginConnection(std::move(stateHandler));
    ShowGameState(ONLINE_JEO_MENU);
}

void
MainWindow::OnJeoOfflineButtonClicked()
{
    m_ui->jeopardyTitleLabel->setText("Jeopardy Offline");
    ShowGameState(OFFLINE_MENU);
}

void
MainWindow::OnBack()
{
    switch(m_gameState)
    {
    case OFFLINE_MENU:
    case ONLINE_JEO_MENU:
        ShowGameState(PICK_JEO_OFFLINE_ONLINE);
        break;

    default:
        break;
    }
}

void
MainWindow::OnOnlineGameStart()
{
    auto stateHandler = std::move(m_ui->connectOnlineWidget->GetStateHandler());
    disconnect(stateHandler.get());

    if(m_gameState == ONLINE_JEO_MENU)
    {
        m_ui->jeopardyGameWidget->StartGame(std::move(stateHandler));
        ShowGameState(JEO_GAME);
    }
    else if( m_gameState == ONLINE_BS_MENU)
    {
        //m_ui->battleshipGameWidget->StartGame(std::move(stateHandler));
        ShowGameState(BS_GAME);
    }
}

void
MainWindow::OnOfflineGameStart()
{
    // Setup the OfflineStateHandler for gamePaneWidget
    m_ui->jeopardyGameWidget->SetAutoPlayEnabled(m_ui->autoPlayCheckBox->isChecked());
    std::unique_ptr<IStateHandler> stateHandler(new StateHandlerOffline);
    m_ui->jeopardyGameWidget->StartGame(std::move(stateHandler));
    ShowGameState(JEO_GAME);
}

void
MainWindow::launchOptionsDialog()
{
    OptionsDialog dlg(this, OptionsData::GetInstance());
    if(dlg.exec() == QDialog::Accepted)
    {
        OptionsData::GetInstance() = dlg.GetOptions();
        m_ui->jeopardyGameWidget->SetOptions(OptionsData::GetInstance());
    }
}

void
MainWindow::ShowGameState(GameState gameState)
{
    m_gameState = gameState;

    switch(m_gameState)
    {
    case PICK_GAME:
        m_ui->pickJeopardyModeWidget->hide();
        m_ui->connectOnlineWidget->hide();
        m_ui->jeopardyGameWidget->hide();
        m_ui->battleshipGameWidget->hide();
        m_ui->pickGameWidget->show();
        break;

    case OFFLINE_MENU:
        m_ui->pickOnlineWidget->hide();
        m_ui->jeopardyGameWidget->hide();
        m_ui->connectOnlineWidget->hide();
        m_ui->pickGameWidget->hide();
        m_ui->battleshipGameWidget->hide();
        m_ui->backButton->show();
        m_ui->optionsWidget->show();
        m_ui->startGameButton->show();
        m_ui->pickJeopardyModeWidget->show();
        break;

    case PICK_JEO_OFFLINE_ONLINE:
        m_ui->backButton->hide();
        m_ui->jeopardyGameWidget->hide();
        m_ui->optionsWidget->hide();
        m_ui->connectOnlineWidget->hide();
        m_ui->startGameButton->hide();
        m_ui->pickGameWidget->hide();
        m_ui->battleshipGameWidget->hide();
        m_ui->jeopardyTitleLabel->setText("Jeopardy");
        m_ui->pickOnlineWidget->show();
        m_ui->pickJeopardyModeWidget->show();
        break;

    case ONLINE_BS_MENU:
    case ONLINE_JEO_MENU:
        m_ui->jeopardyGameWidget->hide();
        m_ui->pickJeopardyModeWidget->hide();
        m_ui->pickGameWidget->hide();
        m_ui->battleshipGameWidget->hide();
        m_ui->connectOnlineWidget->show();
        break;

    case JEO_GAME:
        m_ui->pickJeopardyModeWidget->hide();
        m_ui->connectOnlineWidget->hide();
        m_ui->pickGameWidget->hide();
        m_ui->battleshipGameWidget->hide();
        m_ui->jeopardyGameWidget->show();
        break;

    case BS_GAME:
        m_ui->pickJeopardyModeWidget->hide();
        m_ui->connectOnlineWidget->hide();
        m_ui->pickGameWidget->hide();
        m_ui->jeopardyGameWidget->hide();
        m_ui->battleshipGameWidget->show();
        break;

    default:
        break;
    }
}

void
MainWindow::closeEvent(QCloseEvent *)
{
    OptionsData::GetInstance().Save();
}

MainWindow::~MainWindow() {}
