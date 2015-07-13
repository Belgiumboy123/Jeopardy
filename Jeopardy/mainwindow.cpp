#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "jeopardygame.h"
#include "optionsdialog.h"
#include "pausedialog.h"

#include <QFontDatabase>
#include <QItemDelegate>
#include <QKeyEvent>
#include <QMediaPlayer>
#include <QPainter>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QTimer>

#include <QDebug>

#define CLUE_BLUE "#0A06B3"
#define BOARD_TEXT "#E29D44"
#define CLUE_FONT "Korinna BT"
#define BOARD_FONT "Swiss 911"

#define CLUE_FONT_SIZE 34

class JeopardyItemDelegate : public QStyledItemDelegate
{
public:
    JeopardyItemDelegate(QWidget* parent)
        : QStyledItemDelegate(parent)
    {}

protected:
    virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
    {
        painter->fillRect(option.rect, Qt::black);

        // draw a thicker black border around each item
        // by drawing the clue 3 pixels in from it's original size
        // and letting the black background show through
        // Always draw the item as if the listview is active,
        // this avoids the ugly black/white inactive autoplay selection colors
        QStyleOptionViewItem opt = option;
        opt.rect = option.rect.adjusted(3, 3, -3, -3);
        opt.state |= QStyle::State_Active;
        QStyledItemDelegate::paint(painter,opt,index);
    }
};

/*
 * Derived header class to get us a more useful height
 * and background color
 */
class JeopardyHeader : public QHeaderView
{
public:
    explicit JeopardyHeader(QWidget* parent) : QHeaderView(Qt::Horizontal, parent)
    {
        m_label = new QLabel(parent);
        m_label->setAlignment(Qt::AlignHCenter|Qt::AlignCenter);
        QPalette pal = m_label->palette();
        pal.setColor(m_label->backgroundRole(), QColor(CLUE_BLUE));
        pal.setColor(m_label->foregroundRole(), Qt::white);
        m_label->setPalette(pal);
        m_label->setAutoFillBackground(true);
        m_label->setWordWrap(true);
        m_label->hide();
    }

    virtual QSize sizeHint() const
    {
        QSize size = QHeaderView::sizeHint();
        size.setHeight(120);
        return size;
    }

protected:
    virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
    {
        painter->save();

        painter->fillRect(rect, Qt::black);

        QRect inner = rect.adjusted(3,3,-3,-9);
        painter->fillRect(inner, QBrush(CLUE_BLUE));

        m_label->resize(inner.size());
        m_label->setText( model()->headerData(logicalIndex,Qt::Horizontal, Qt::DisplayRole).toString() );
        m_label->render(painter,inner.topLeft());

        painter->restore();
    }

private:
    QLabel* m_label;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_options(OptionsData::FromSettings())
    , m_timeIntervals(m_options.m_timeIntervals)
    , m_game( new JeopardyGame(m_options.m_nextClueOptions) )
{
    m_ui->setupUi(this);

    // Load the two needed fonts from resources.qrc
    QFontDatabase::addApplicationFont(":/korinna");
    QFontDatabase::addApplicationFont(":/swiss");

    QPalette pal = m_ui->bottomWidget->palette();
    pal.setColor( m_ui->bottomWidget->backgroundRole(), Qt::yellow);
    m_ui->bottomWidget->setAutoFillBackground(true);
    m_ui->bottomWidget->setPalette(pal);

    pal = m_ui->headerWidget->palette();
    pal.setColor(m_ui->headerWidget->backgroundRole(), Qt::red);
    m_ui->headerWidget->setAutoFillBackground(true);
    m_ui->headerWidget->setPalette(pal);

    m_ui->bottomWidget->hide();
    m_ui->headerWidget->hide();
    m_ui->mainToolBar->hide();

    m_ui->tableView->setModel(m_game->GetModel());
    m_ui->tableView->setHorizontalHeader(new JeopardyHeader(m_ui->tableView));
    m_ui->tableView->setShowGrid(false);
    m_ui->tableView->setItemDelegate(new JeopardyItemDelegate(this));
    m_ui->tableView->verticalHeader()->hide();
    m_ui->tableView->setCornerButtonEnabled(false);
    m_ui->tableView->setTextElideMode(Qt::ElideNone);
    m_ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui->tableView->setDragDropOverwriteMode(false);
    m_ui->tableView->setDropIndicatorShown(false);
    m_ui->tableView->setAutoScroll(false);
    m_ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->tableView->setDragEnabled(false);
    m_ui->tableView->setFrameShape(QFrame::NoFrame);
    m_ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_ui->tableView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ui->tableView->installEventFilter(this);

    QFont boardFont( BOARD_FONT, 34, QFont::Normal );
    boardFont.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    m_ui->tableView->setFont(boardFont);

    connect( m_ui->tableView, &QAbstractItemView::clicked, this, &MainWindow::handleBoardClick );

    m_ui->clueLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    m_ui->clueLabel->setWordWrap(true);

    QFont clueFont( CLUE_FONT, CLUE_FONT_SIZE, QFont::Normal );
    m_ui->clueWidget->setFont(clueFont);
    auto cluePal = m_ui->clueWidget->palette();
    cluePal.setColor(m_ui->clueWidget->backgroundRole(), QColor(CLUE_BLUE));
    cluePal.setColor(m_ui->clueWidget->foregroundRole(), Qt::white);
    m_ui->clueWidget->setPalette(cluePal);
    m_ui->clueWidget->setAutoFillBackground(true);
    m_ui->clueWidget->installEventFilter(this);

    auto jeopardyTitleLabelPal = m_ui->jeopardyTitleLabel->palette();
    jeopardyTitleLabelPal.setColor(m_ui->jeopardyTitleLabel->foregroundRole(), BOARD_TEXT);
    m_ui->jeopardyTitleLabel->setPalette(jeopardyTitleLabelPal);
    boardFont.setPointSize(200);
    m_ui->jeopardyTitleLabel->setFont(boardFont);

    clueFont.setPointSize(40);
    m_ui->startGameButton->setFont(clueFont);
    connect( m_ui->startGameButton, &QPushButton::clicked, this, &MainWindow::handleStartGameClick );

    m_ui->pickGameWidget->setAutoFillBackground(true);
    m_ui->pickGameWidget->setPalette(cluePal);

    clueFont.setPointSize(28);
    m_ui->autoPlayCheckBox->setFont(clueFont);

    clueFont.setPointSize(20);
    m_ui->optionsButton->setFont(clueFont);
    connect( m_ui->optionsButton, &QPushButton::clicked, this, &MainWindow::launchOptionsDialog );

    m_ui->optionsWidget->setFixedSize(m_ui->optionsWidget->sizeHint());

    // setup up media player with final jeopardy song
    m_mediaPlayer = new QMediaPlayer(this);
    m_mediaPlayer->setMedia(QUrl::fromLocalFile(DatabaseUtils::GetFilePathAppResourcesFile("song.mp3")));
    UpdateMediaPlayerFromOptions();

    // start with menu mode
    m_ui->tableView->hide();
    m_ui->clueWidget->hide();

    m_mode = MENU;

    showMaximized();
}

bool
MainWindow::IsAutoPlayEnabled() const
{
    return m_ui->autoPlayCheckBox->isChecked();
}

void
MainWindow::handleStartGameClick()
{
    m_game->LoadRandomGame();

    m_mode = BOARD;
    m_ui->pickGameWidget->hide();
    m_ui->tableView->show();
    m_ui->tableView->setFocus();

    // if auto play is enabled, pick the first clue
    if( IsAutoPlayEnabled())
    {
        // pick an index to start animating from
        m_clickedIndex = m_ui->tableView->model()->index(0, 0);
        m_ui->tableView->selectionModel()->setCurrentIndex(m_clickedIndex, QItemSelectionModel::Select);

        AutoPlayNextClue();
    }
}

void
MainWindow::handleBoardClick(const QModelIndex& index)
{
    // cancel the auto-play and time-over timers
    // and pick the clue the user just clicked
    if( m_autoPlayTimer && m_autoPlayTimer->isActive())
    {
        m_autoPlayTimer->stop();
    }

    if( m_timeOverTimer && m_timeOverTimer->isActive())
    {
        m_timeOverTimer->stop();
    }

    const QString& question = m_game->HandleBoardAction( index );

    if( question.isEmpty() )
        return;

    SetNewClueQuestion(index, question);
}

void
MainWindow::SetNewClueQuestion(const QModelIndex& index, const QString& question)
{
    // save clicked index here, the tableview active index can't be trusted
    m_clickedIndex = index;

    // set label text
    m_ui->clueLabel->setText( question );

    StartClueTimer(m_timeIntervals.ClueQuestion);

    // hide the board
    m_ui->tableView->hide();

    // show the label
    m_ui->clueWidget->show();

    m_mode = CLUE_QUESTION;
}

void
MainWindow::handleClueClick()
{
    if( m_clueTimer && m_clueTimer->isActive())
    {
        m_clueTimer->stop();
    }

    if( m_timeOverTimer && m_timeOverTimer->isActive())
    {
        m_timeOverTimer->stop();
    }

    if( m_mode == CLUE_QUESTION)
    {
        // reset the text color in case it was changed
        // hitting a time out.
        auto cluePal = m_ui->clueWidget->palette();
        cluePal.setColor(m_ui->clueWidget->foregroundRole(), Qt::white);
        m_ui->clueWidget->setPalette(cluePal);

        const QString& answer = m_game->HandleClueAction(m_clickedIndex);
        m_ui->clueLabel->setText( answer );

        m_mode = CLUE_ANSWER;

        if( IsAutoPlayEnabled())
        {
            StartTimeOverTimer(m_timeIntervals.ClueAnswer);
        }
    }
    else if( m_mode == CLUE_ANSWER)
    {
        auto newMode = m_game->HandleAnswerAction();

        if( newMode == JeopardyGame::GM_FINAL )
        {
            // show final jeapardy start screen for 3 seconds / optional click

            // Make font size a little bigger
            auto font = m_ui->clueLabel->font();
            font.setPointSize(CLUE_FONT_SIZE+30);
            m_ui->clueLabel->setFont(font);

            m_ui->clueLabel->setText( "Final Jeopardy!" );
            m_mode = FINAL_START;

            StartTimeOverTimer(m_timeIntervals.FinalStart);
        }
        else
        {
            m_ui->clueWidget->hide();

            // When loading to double jeopardy
            // need to reset m_clickedIndex because it needs to point to the
            // newly added widgets in the model.
            if( newMode == JeopardyGame::GM_DOUBLE)
            {
                m_clickedIndex = m_ui->tableView->model()->index(0, 0);
                m_ui->tableView->selectionModel()->setCurrentIndex(m_clickedIndex, QItemSelectionModel::Select);
            }
            else
            {
                m_ui->tableView->selectionModel()->setCurrentIndex(m_clickedIndex, QItemSelectionModel::NoUpdate);
                m_ui->tableView->clearSelection();
            }

            m_ui->tableView->show();
            m_mode = BOARD;

            if(IsAutoPlayEnabled())
            {
                AutoPlayNextClue();
            }
        }
    }
    else if( m_mode == FINAL_START)
    {
        // reset the font size
        auto font = m_ui->clueLabel->font();
        font.setPointSize(CLUE_FONT_SIZE);
        m_ui->clueLabel->setFont(font);

        // show final jeopardy category
        m_ui->clueLabel->setText( m_game->GetFinalCategory() );
        m_mode = FINAL_CATEGORY;

        StartTimeOverTimer(m_timeIntervals.FinalCategory);
    }
    else if( m_mode == FINAL_CATEGORY)
    {
         // show final jeapardy clue and pause for 30 seconds / optional click
        m_ui->clueLabel->setText( m_game->GetFinalClue() );
        m_mode = FINAL_CLUE;

        StartClueTimer(m_timeIntervals.FinalQuestion);

        // start the final jeopardy music
        m_mediaPlayer->play();
    }
    else if( m_mode == FINAL_CLUE)
    {
        // stop the final jeopardy music
        m_mediaPlayer->stop();

        auto cluePal = m_ui->clueWidget->palette();
        cluePal.setColor(m_ui->clueWidget->foregroundRole(), Qt::white);
        m_ui->clueWidget->setPalette(cluePal);

        // show final jeapardy answer wait for user click -> back to start screen
        m_ui->clueLabel->setText( m_game->GetFinalAnswer() );
        m_mode = FINAL_ANSWER;

        if(IsAutoPlayEnabled())
        {
            StartTimeOverTimer(m_timeIntervals.FinalAnswer);
        }
    }
    else if(m_mode == FINAL_ANSWER )
    {
        // back to showing pick a game menu
        m_ui->clueLabel->setText("Game Over\nSorry Jillian, better luck next time.");

        m_mode = GAME_OVER;

        if(IsAutoPlayEnabled())
        {
            StartTimeOverTimer(m_timeIntervals.GameOver);
        }
    }
    else if( m_mode == GAME_OVER )
    {
        // show start game screen
        m_mode = MENU;
        m_ui->clueWidget->hide();
        m_ui->pickGameWidget->show();
    }
}

void
MainWindow::AutoPlayNextClue()
{
    m_autoPlayState.newIndex = m_game->GetNextClue(m_clickedIndex);
    m_autoPlayState.currColumn = m_clickedIndex.column();
    m_autoPlayState.currRow = m_clickedIndex.row();
    m_autoPlayState.columnDirection = m_autoPlayState.currColumn < m_autoPlayState.newIndex.column() ? 1 : -1;
    m_autoPlayState.rowDirection = m_autoPlayState.currRow < m_autoPlayState.newIndex.row() ? 1 : -1;

    // change the mode so we can correctly handle user interaction
    // during the animation
    m_mode = CLUE_ANIMATION;

    StartAutoPlayTimer();
}

void
MainWindow::StartAutoPlayTimer()
{
    m_autoPlayTimer = new QTimer(this);
    m_autoPlayTimer->setSingleShot(true);
    m_autoPlayTimer->setInterval(m_timeIntervals.AutoPlayAnimation);
    connect( m_autoPlayTimer, &QTimer::timeout, this, &MainWindow::OnAutoPlayTimer);
    m_autoPlayTimer->start();
}

void
MainWindow::OnAutoPlayTimer()
{
    if( m_autoPlayState.currColumn != m_autoPlayState.newIndex.column())
    {
        m_autoPlayState.currColumn += m_autoPlayState.columnDirection;
    }
    else if( m_autoPlayState.currRow != m_autoPlayState.newIndex.row() )
    {
        m_autoPlayState.currRow += m_autoPlayState.rowDirection;
    }
    else
    {
        // we have reached newIndex
        // start a timer to open up that clue
        m_timeOverTimer = new QTimer(this);
        m_timeOverTimer->setInterval(m_timeIntervals.AutoPlayFinal);
        m_timeOverTimer->setSingleShot(true);
        connect(m_timeOverTimer, &QTimer::timeout, this, [&]()
            {
                handleBoardClick(m_autoPlayState.newIndex);
            });
        m_timeOverTimer->start();
        return;
    }

    // if we have reached this code it means we haven't reached newIndex yet, so try again
    auto stepIndex = m_ui->tableView->model()->index(m_autoPlayState.currRow, m_autoPlayState.currColumn);
    m_ui->tableView->selectionModel()->setCurrentIndex(stepIndex,QItemSelectionModel::ClearAndSelect);
    StartAutoPlayTimer();
}

void
MainWindow::OnClueTimerOut()
{
    auto cluePal = m_ui->clueWidget->palette();
    cluePal.setColor(m_ui->clueWidget->foregroundRole(), Qt::red);
    m_ui->clueWidget->setPalette(cluePal);

    m_ui->clueLabel->setText("Time is Up!");

    StartTimeOverTimer(m_timeIntervals.ClueTimeOut);
}

void
MainWindow::StartTimeOverTimer(const unsigned int milliSeconds)
{
    m_timeOverTimer = new QTimer(this);
    m_timeOverTimer->setInterval(milliSeconds);
    m_timeOverTimer->setSingleShot(true);
    connect(m_timeOverTimer, &QTimer::timeout, this, &MainWindow::OnTimeOverTimerOut);
    m_timeOverTimer->start();
}

void
MainWindow::OnTimeOverTimerOut()
{
    handleClueClick();
}

bool
MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if( watched == m_ui->clueWidget)
    {
        if( event->type() == QEvent::MouseButtonPress)
        {
            handleClueClick();
            return true;
        }
        else if( event->type() == QEvent::KeyRelease)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if( keyEvent->key() == Qt::Key_Escape)
            {
                launchPauseDialog();
                return true;
            }
        }
    }
    else if( watched == m_ui->tableView)
    {
        if( event->type() == QEvent::KeyRelease)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            switch( keyEvent->key())
            {
            case Qt::Key_Return:
            case Qt::Key_Enter:
                if( m_mode == BOARD)
                {
                    handleBoardClick(m_ui->tableView->currentIndex());
                }
                else if( m_mode != CLUE_ANIMATION)
                {
                    handleClueClick();
                }
                return true;

            // this cancels arrow key events when we are
            // in middle of animation auto-playing to uor next clue.
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Right:
            case Qt::Key_Left:
                if( m_mode == CLUE_ANIMATION)
                    return true;
                break;

            case Qt::Key_Escape:
                launchPauseDialog();
                return true;
            }
        }
    }

    return QMainWindow::eventFilter(watched,event);
}

void
MainWindow::StartClueTimer( const unsigned int milliSeconds)
{
    m_clueTimer = new QTimer(this);
    m_clueTimer->setInterval(milliSeconds);
    m_clueTimer->setSingleShot(true);
    connect(m_clueTimer, &QTimer::timeout, this, &MainWindow::OnClueTimerOut);
    m_clueTimer->start();
}

void
MainWindow::launchOptionsDialog()
{
    OptionsDialog dlg(this, m_options);
    if(dlg.exec() == QDialog::Accepted)
    {
        m_options = dlg.GetOptions();
        UpdateFromOptions();
    }
}

void
MainWindow::UpdateMediaPlayerFromOptions()
{
    m_mediaPlayer->setVolume(m_options.m_music.volume);
    m_mediaPlayer->setMuted(!m_options.m_music.playFinalJeopardy);
}

void
MainWindow::UpdateFromOptions()
{
    m_timeIntervals = m_options.m_timeIntervals;
    UpdateMediaPlayerFromOptions();
    m_game->SetNextClueOptions(m_options.m_nextClueOptions);
}

void
MainWindow::launchPauseDialog()
{
    if( m_mode != PAUSED && m_mode != MENU)
    {
        GameState originalMode = m_mode;
        m_mode = PAUSED;

        auto setTimeLeft = [](QTimer* timer, int& timeLeft){
            timeLeft = 0;
            if( timer && timer->isActive())
            {
                timeLeft = timer->remainingTime();
                if( timeLeft > -1 )
                {
                    timer->stop();
                }
            }
        };

         // stop all timers and record any remaining time
        int clueTimeLeft;
        setTimeLeft( m_clueTimer, clueTimeLeft);

        int timeOverLeft;
        setTimeLeft( m_timeOverTimer, timeOverLeft);

        int autoPlayLeft;
        setTimeLeft(m_autoPlayTimer, autoPlayLeft);

        auto mediaPosition = m_mediaPlayer->position();
        m_mediaPlayer->stop();

        // initialize dialog and set its colors
        PauseDialog dlg(this, QColor(BOARD_TEXT), m_options);
        if( dlg.exec() == QDialog::Accepted)
        {
            m_options = dlg.GetOptions();
            UpdateFromOptions();

            m_mode = originalMode;

            // restart any timers that had time left
            if( m_mode == FINAL_CLUE)
            {
                m_mediaPlayer->setPosition(mediaPosition);
                m_mediaPlayer->play();
            }

            if( clueTimeLeft > 0)
            {
                StartClueTimer(clueTimeLeft);
            }

            if( timeOverLeft > 0)
            {
                StartTimeOverTimer(timeOverLeft);
            }

            if( autoPlayLeft > 0)
            {
                StartAutoPlayTimer();
            }
        }
        else
        {
            // gracefully quit the game and return to main screen
            m_mode = MENU;
            m_ui->tableView->hide();
            m_ui->clueWidget->hide();
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
