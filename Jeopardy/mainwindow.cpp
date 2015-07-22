#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "optionsdialog.h"
#include "pausedialog.h"

#include <QFontDatabase>
#include <QKeyEvent>

#define CLUE_BLUE "#0A06B3"
#define BOARD_TEXT "#E29D44"
#define CLUE_FONT "Korinna BT"
#define BOARD_FONT "Swiss 911"

#define CLUE_FONT_SIZE 34

MainWindow::MainWindow(QWidget *parent/*=nullptr*/)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_options(OptionsData::FromSettings())
{
    m_ui->setupUi(this);

    // Load the two needed fonts from resources.qrc
    QFontDatabase::addApplicationFont(":/korinna");
    QFontDatabase::addApplicationFont(":/swiss");

    // the toolbar is unused for now
    m_ui->mainToolBar->hide();

    // TODO move fonts to common location
    QFont boardFont( BOARD_FONT, 34, QFont::Normal );
    boardFont.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    QFont clueFont( CLUE_FONT, CLUE_FONT_SIZE, QFont::Normal );

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
    m_ui->gamePaneWidget->SetOptions(m_options);
    m_ui->gamePaneWidget->installEventFilter(this);

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

    m_ui->gamePaneWidget->StartGame();
    m_ui->gamePaneWidget->show();
}

bool
MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if( watched == m_ui->gamePaneWidget)
    {
        if( event->type() == QEvent::KeyRelease)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if( keyEvent->key() == Qt::Key_Escape)
            {
                launchPauseDialog();
                return true;
            }
        }
    }

    return QMainWindow::eventFilter(watched,event);
}

void
MainWindow::launchOptionsDialog()
{
    OptionsDialog dlg(this, m_options);
    if(dlg.exec() == QDialog::Accepted)
    {
        m_options = dlg.GetOptions();
        m_ui->gamePaneWidget->SetOptions(m_options);
    }
}

void
MainWindow::launchPauseDialog()
{
    if( m_mode == GAME)
    {
        m_mode = PAUSED;

        m_ui->gamePaneWidget->PauseGame();

        // initialize dialog and set its colors
        PauseDialog dlg(this, QColor(BOARD_TEXT), m_options);

        auto dialogReturnCode = dlg.exec();

        // we update the options if the user changes them
        // regardless if they quit the game or continued
        if( dlg.HaveOptionsChanged() )
        {
            m_options = dlg.GetOptions();
            m_ui->gamePaneWidget->SetOptions(m_options);
        }

        if( dialogReturnCode == QDialog::Accepted)
        {
            m_mode = GAME;
            m_ui->gamePaneWidget->ContinueGame();
        }
        else
        {
            // gracefully quit the game and return to main screen
            m_mode = MENU;
            m_ui->gamePaneWidget->hide();
            m_ui->pickGameWidget->show();
        }
    }
}

void
MainWindow::closeEvent(QCloseEvent *)
{
    m_options.Save();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
