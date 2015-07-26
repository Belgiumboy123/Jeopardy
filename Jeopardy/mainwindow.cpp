#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "optionsdialog.h"
#include "pausedialog.h"
#include "qtutility.h"
#include "statehandleroffline.h"

#include <QFontDatabase>
#include <QKeyEvent>

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

    clueFont.setPointSize(40);
    m_ui->startGameButton->setFont(clueFont);
    connect( m_ui->startGameButton, &QPushButton::clicked, this, &MainWindow::handleStartGameClick );

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

    connect( m_ui->autoPlayCheckBox, &QCheckBox::toggled, m_ui->gamePaneWidget, &GamePaneWidget::SetAutoPlayEnabled);

    // start with menu mode
    m_ui->gamePaneWidget->hide();
    m_mode = MENU;

    showMaximized();
}

void
MainWindow::OnGameOver()
{
    m_mode = MENU;
    m_ui->gamePaneWidget->hide();
    m_ui->pickGameWidget->show();
}

void
MainWindow::handleStartGameClick()
{
    m_mode = GAME;
    m_ui->pickGameWidget->hide();

    std::unique_ptr<IStateHandler> stateHandler(new StateHandlerOffline);
    m_ui->gamePaneWidget->StartGame(std::move(stateHandler));
    m_ui->gamePaneWidget->show();
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
MainWindow::closeEvent(QCloseEvent *)
{
    OptionsData::GetInstance().Save();
}

MainWindow::~MainWindow() {}
