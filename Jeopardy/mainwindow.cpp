#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "optionsdialog.h"
#include "pausedialog.h"
#include "qtutility.h"
#include "statehandleroffline.h"

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

    // the toolbar is unused for now
    m_ui->mainToolBar->hide();

    QFont boardFont( QtUtil::GetBasicBoardFont() );
    QFont clueFont( QtUtil::GetBasicClueFont() );

    auto jeopardyTitleLabelPal = m_ui->jeopardyTitleLabel->palette();
    jeopardyTitleLabelPal.setColor(m_ui->jeopardyTitleLabel->foregroundRole(), BOARD_TEXT);
    m_ui->jeopardyTitleLabel->setPalette(jeopardyTitleLabelPal);
    boardFont.setPointSize(200);
    m_ui->jeopardyTitleLabel->setFont(boardFont);

    QFont buttonFont(clueFont);
    buttonFont.setPointSize(40);
    m_ui->startGameButton->setFont(buttonFont);
    connect( m_ui->startGameButton, &QPushButton::clicked, this, &MainWindow::OnOfflineGameStart );

   // m_ui->backButton->setFont(buttonFont);
    //connect( m_ui->backButton, &QPushButton::clicked, this, &MainWindow::OnBack);

    m_ui->pickGameWidget->setAutoFillBackground(true);
    auto pickGamePal = m_ui->pickGameWidget->palette();
    pickGamePal.setColor(m_ui->pickGameWidget->backgroundRole(), QColor(CLUE_BLUE));
    pickGamePal.setColor(m_ui->pickGameWidget->foregroundRole(), Qt::white);
    m_ui->pickGameWidget->setPalette(pickGamePal);

    clueFont.setPointSize(28);
    m_ui->autoPlayCheckBox->setFont(clueFont);

    clueFont.setPointSize(20);
    m_ui->optionsButton->setFont(clueFont);
    connect( m_ui->optionsButton, &QPushButton::clicked, this, &MainWindow::launchOptionsDialog );

    // make sure everything is packed together nicely
    m_ui->optionsWidget->setFixedSize(m_ui->optionsWidget->sizeHint());

    // Lets us know when a game is over
    connect( m_ui->gamePaneWidget, &GamePaneWidget::GameOver, this, &MainWindow::OnGameOver);
    m_ui->gamePaneWidget->SetOptions(OptionsData::GetInstance());

    connect( m_ui->connectOnlineWidget, &ConnectOnlineWidget::BackToMenu, this, &MainWindow::OnBack);
    connect( m_ui->connectOnlineWidget, &ConnectOnlineWidget::StartGame, this, &MainWindow::OnOnlineGameStart);

    connect( m_ui->autoPlayCheckBox, &QCheckBox::toggled, m_ui->gamePaneWidget, &GamePaneWidget::SetAutoPlayEnabled);

    m_ui->onlineButton->setFont(buttonFont);
    connect(  m_ui->onlineButton, &QPushButton::clicked, this, &MainWindow::OnOnlineButtonClicked);

    m_ui->offlineButton->setFont(buttonFont);
    connect( m_ui->offlineButton, &QPushButton::clicked, this, &MainWindow::OnOfflineButtonClicked);

    ShowGameState(PICK_OFFLINE_ONLINE);
    showMaximized();
}

void
MainWindow::OnGameOver()
{
    ShowGameState(PICK_OFFLINE_ONLINE);
}

void
MainWindow::OnOnlineButtonClicked()
{
    ShowGameState(ONLINE_MENU);
}

void
MainWindow::OnOfflineButtonClicked()
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
    case ONLINE_MENU:
        ShowGameState(PICK_OFFLINE_ONLINE);
        break;

    default:
        break;
    }
}

void
MainWindow::OnOnlineGameStart()
{
    // Grab/create a StateHandlerOnline here.
    // m_ui->gamePaneWidget->StartGame(std::move(stateHandler));
    // ShowGameState(Game)
}

void
MainWindow::OnOfflineGameStart()
{
    // Setup the OfflineStateHandler for gamePaneWidget
    std::unique_ptr<IStateHandler> stateHandler(new StateHandlerOffline);
    m_ui->gamePaneWidget->StartGame(std::move(stateHandler));

    ShowGameState(GAME);
}

void
MainWindow::launchOptionsDialog()
{
    OptionsDialog dlg(this, OptionsData::GetInstance());
    if(dlg.exec() == QDialog::Accepted)
    {
        OptionsData::GetInstance() = dlg.GetOptions();
        m_ui->gamePaneWidget->SetOptions(OptionsData::GetInstance());
    }
}

void
MainWindow::ShowGameState(GameState gameState)
{
    m_gameState = gameState;

    switch(m_gameState)
    {
    case OFFLINE_MENU:
        m_ui->pickOnlineWidget->hide();
        m_ui->gamePaneWidget->hide();
        m_ui->connectOnlineWidget->hide();
        //m_ui->backButton->show();
        m_ui->optionsWidget->show();
        m_ui->startGameButton->show();
        m_ui->pickGameWidget->show();
        break;

    case PICK_OFFLINE_ONLINE:
        //m_ui->backButton->hide();
        m_ui->gamePaneWidget->hide();
        m_ui->optionsWidget->hide();
        m_ui->connectOnlineWidget->hide();
        m_ui->startGameButton->hide();
        m_ui->jeopardyTitleLabel->setText("Jeopardy");
        m_ui->pickOnlineWidget->show();
        m_ui->pickGameWidget->show();
        break;

    case ONLINE_MENU:
        m_ui->gamePaneWidget->hide();
        m_ui->pickGameWidget->hide();
        m_ui->connectOnlineWidget->show();
        break;

    case GAME:
        m_ui->pickGameWidget->hide();
        m_ui->connectOnlineWidget->hide();
        m_ui->gamePaneWidget->show();
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
