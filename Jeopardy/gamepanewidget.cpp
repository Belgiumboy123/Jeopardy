#include "gamepanewidget.h"
#include "ui_gamepanewidget.h"

#include "jeopardygame.h"
#include "pausedialog.h"
#include "qtutility.h"
#include "istatehandler.h"

#include <QItemDelegate>
#include <QKeyEvent>
#include <QMediaPlayer>
#include <QPainter>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QTimer>

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

GamePaneWidget::GamePaneWidget(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::GamePaneWidget)
  , m_options(OptionsData::GetInstance())
  , m_timeIntervals(m_options.m_timeIntervals)
  , m_clueTimer(nullptr)
  , m_timeOverTimer(nullptr)
  , m_isAutoPlayEnabled(false)
  , m_autoPlayTimer(nullptr)
  , m_mediaPlayer(nullptr)
{
    m_ui->setupUi(this);

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
    m_ui->tableView->setFont(QtUtil::GetBasicBoardFont());

    m_ui->clueLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    m_ui->clueLabel->setWordWrap(true);

    m_ui->clueWidget->setFont(QtUtil::GetBasicClueFont());
    auto cluePal = m_ui->clueWidget->palette();
    cluePal.setColor(m_ui->clueWidget->backgroundRole(), QColor(CLUE_BLUE));
    cluePal.setColor(m_ui->clueWidget->foregroundRole(), Qt::white);
    m_ui->clueWidget->setPalette(cluePal);
    m_ui->clueWidget->setAutoFillBackground(true);

    m_mediaPlayer = std::unique_ptr<QMediaPlayer>( new QMediaPlayer( this ) );
    m_mediaPlayer->setMedia(QUrl::fromLocalFile(DatabaseUtils::GetFilePathAppResourcesFile("song.mp3")));
}

void
GamePaneWidget::SetOptions(const OptionsData& options)
{
    m_options = options;
    m_timeIntervals = m_options.m_timeIntervals;
    UpdateMediaPlayerFromOptions();
    if(m_stateHandler)
    {
        m_stateHandler->SetNextClueOptions(m_options.m_nextClueOptions);
    }
}

bool
GamePaneWidget::IsAutoPlayEnabled() const
{
    return m_isAutoPlayEnabled;
}

void
GamePaneWidget::SetAutoPlayEnabled(bool flag)
{
    m_isAutoPlayEnabled = flag;
}

void
GamePaneWidget::OnStateChanged(GameStateUtils::GameState state, const QModelIndex& index, const QString& message)
{
    switch(state)
    {
    case GameStateUtils::GameState::BOARD_START:
    {
        m_ui->clueWidget->hide();
        m_ui->tableView->show();
        m_ui->tableView->setFocus();

        m_mode = BOARD;

        if( IsAutoPlayEnabled())
        {
            // pick an index to start animating from
            m_clickedIndex = index;

            // The following both caus non-consistent crashes, this means user can't tell what the first selected clue is
            //m_ui->tableView->selectionModel()->setCurrentIndex(m_clickedIndex, QItemSelectionModel::Select);
            //m_ui->tableView->setCurrentIndex(m_clickedIndex);

            AutoPlayNextClue(index);
        }
        break;
    }

    case GameStateUtils::GameState::BOARD:
    {
        m_ui->clueWidget->hide();
        m_mode = BOARD;

        m_ui->tableView->selectionModel()->setCurrentIndex(m_clickedIndex, QItemSelectionModel::NoUpdate);
        m_ui->tableView->clearSelection();
        m_ui->tableView->show();
        m_ui->tableView->setFocus();

        if( IsAutoPlayEnabled())
        {
            AutoPlayNextClue(index);
        }
        break;
    }

    case GameStateUtils::GameState::CLUE_QUESTION:
    {
        // save clicked index here, the tableview active index can't be trusted
        m_clickedIndex = index;

        m_ui->clueLabel->setText( message );

        StartClueTimer(m_timeIntervals.ClueQuestion);

        m_ui->tableView->hide();
        m_ui->clueWidget->show();

        m_mode = CLUE_QUESTION;
        break;
    }

    case GameStateUtils::GameState::CLUE_ANSWER:
    {
        // reset the text color in case it was changed
        // hitting a time out.
        auto cluePal = m_ui->clueWidget->palette();
        cluePal.setColor(m_ui->clueWidget->foregroundRole(), Qt::white);
        m_ui->clueWidget->setPalette(cluePal);

        m_ui->clueLabel->setText(message/*message is the clue answer*/ );

        m_mode = CLUE_ANSWER;

        if( IsAutoPlayEnabled())
        {
            StartTimeOverTimer(m_timeIntervals.ClueAnswer);
        }
        break;
    }

    case GameStateUtils::GameState::FINAL_START:
    {
        // Make font size a little bigger
        auto font = m_ui->clueLabel->font();
        font.setPointSize(CLUE_FONT_SIZE+30);
        m_ui->clueLabel->setFont(font);

        m_ui->clueLabel->setText( "Final Jeopardy!" );
        m_mode = FINAL_START;

        StartTimeOverTimer(m_timeIntervals.FinalStart);
        break;
    }

    case GameStateUtils::GameState::FINAL_CATEGORY:
    {
        // reset the font size
        auto font = m_ui->clueLabel->font();
        font.setPointSize(CLUE_FONT_SIZE);
        m_ui->clueLabel->setFont(font);

        m_ui->clueLabel->setText(message/*message is final category*/);
        m_mode = FINAL_CATEGORY;

        StartTimeOverTimer(m_timeIntervals.FinalCategory);
        break;
    }

    case GameStateUtils::GameState::FINAL_CLUE:
    {
        // show final jeapardy clue and pause for 30 seconds / optional click
        m_ui->clueLabel->setText(message);
        m_mode = FINAL_CLUE;

        StartClueTimer(m_timeIntervals.FinalQuestion);

        m_mediaPlayer->play();
        break;
    }

    case GameStateUtils::GameState::FINAL_ANSWER:
    {
        m_mediaPlayer->stop();

        auto cluePal = m_ui->clueWidget->palette();
        cluePal.setColor(m_ui->clueWidget->foregroundRole(), Qt::white);
        m_ui->clueWidget->setPalette(cluePal);

        m_ui->clueLabel->setText(message);
        m_mode = FINAL_ANSWER;

        if(IsAutoPlayEnabled())
        {
            StartTimeOverTimer(m_timeIntervals.FinalAnswer);
        }
        break;
    }

    case GameStateUtils::GameState::GAME_OVER:
    {
        m_ui->clueLabel->setText("Game Over\nSorry Jillian, better luck next time.");

        m_mode = GAME_OVER;

        if(IsAutoPlayEnabled())
        {
            StartTimeOverTimer(m_timeIntervals.GameOver);
        }
    }

    default:
        break;
    }
}

void
GamePaneWidget::StartGame(std::unique_ptr<IStateHandler> stateHandler)
{
     m_mode = MENU;

    // Setup the state handler
    m_stateHandler = std::move(stateHandler);
    connect( m_stateHandler.get(), &IStateHandler::StateChanged, this, &GamePaneWidget::OnStateChanged);
    m_ui->tableView->setModel(m_stateHandler->GetModel());

    if( m_stateHandler->AllowUserInteraction())
    {
        m_ui->tableView->installEventFilter(this);
        m_ui->clueWidget->installEventFilter(this);
        connect( m_ui->tableView, &QAbstractItemView::clicked, this, &GamePaneWidget::handleBoardClick );
    }
    else
    {
        // if User can't interact it means auto play is on by default
        SetAutoPlayEnabled(true);
        m_ui->tableView->removeEventFilter(this);
        m_ui->clueWidget->removeEventFilter(this);
        disconnect( m_ui->tableView, &QAbstractItemView::clicked, this, &GamePaneWidget::handleBoardClick );
    }

    m_stateHandler->SetNextClueOptions(m_options.m_nextClueOptions);
    m_stateHandler->DoActionOnState(GameStateUtils::GameState::MENU);
}

void
GamePaneWidget::handleBoardClick(const QModelIndex& index)
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

    m_stateHandler->DoActionOnState(GameStateUtils::GameState::BOARD, index);
}

void
GamePaneWidget::handleClueClick()
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
        m_stateHandler->DoActionOnState(GameStateUtils::GameState::CLUE_QUESTION, m_clickedIndex);
    }
    else if( m_mode == CLUE_ANSWER)
    {
        m_stateHandler->DoActionOnState(GameStateUtils::GameState::CLUE_ANSWER, m_clickedIndex);
    }
    else if( m_mode == FINAL_START)
    {
        m_stateHandler->DoActionOnState(GameStateUtils::GameState::FINAL_START);
    }
    else if( m_mode == FINAL_CATEGORY)
    {
        m_stateHandler->DoActionOnState(GameStateUtils::GameState::FINAL_CATEGORY);
    }
    else if( m_mode == FINAL_CLUE)
    {
        m_stateHandler->DoActionOnState(GameStateUtils::GameState::FINAL_CLUE);
    }
    else if(m_mode == FINAL_ANSWER )
    {
        m_stateHandler->DoActionOnState(GameStateUtils::GameState::FINAL_ANSWER);
    }
    else if( m_mode == GAME_OVER )
    {
        // let our listeners know its game over
        m_mode = MENU;
        emit GameOver();
    }
}

void
GamePaneWidget::AutoPlayNextClue(const QModelIndex& index)
{
    m_autoPlayState.newIndex = index;
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
GamePaneWidget::StartAutoPlayTimer()
{
    m_autoPlayTimer = new QTimer(this);
    m_autoPlayTimer->setSingleShot(true);
    m_autoPlayTimer->setInterval(m_timeIntervals.AutoPlayAnimation);
    connect( m_autoPlayTimer, &QTimer::timeout, this, &GamePaneWidget::OnAutoPlayTimer);
    m_autoPlayTimer->start();
}

void
GamePaneWidget::OnAutoPlayTimer()
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
GamePaneWidget::OnClueTimerOut()
{
    auto cluePal = m_ui->clueWidget->palette();
    cluePal.setColor(m_ui->clueWidget->foregroundRole(), Qt::red);
    m_ui->clueWidget->setPalette(cluePal);

    m_ui->clueLabel->setText("Time is Up!");

    StartTimeOverTimer(m_timeIntervals.ClueTimeOut);
}

void
GamePaneWidget::StartTimeOverTimer(const unsigned int milliSeconds)
{
    m_timeOverTimer = new QTimer(this);
    m_timeOverTimer->setInterval(milliSeconds);
    m_timeOverTimer->setSingleShot(true);
    connect(m_timeOverTimer, &QTimer::timeout, this, &GamePaneWidget::OnTimeOverTimerOut);
    m_timeOverTimer->start();
}

void
GamePaneWidget::OnTimeOverTimerOut()
{
    handleClueClick();
}

bool
GamePaneWidget::eventFilter(QObject* watched, QEvent* event)
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

    return QWidget::eventFilter(watched,event);
}

void
GamePaneWidget::launchPauseDialog()
{
    if( m_mode != MENU && m_mode != PAUSED)
    {
        auto originalMode = m_mode;
        m_mode = PAUSED;

        PauseState pauseState;
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
        setTimeLeft( m_clueTimer, pauseState.clueTimeLeft);
        setTimeLeft( m_timeOverTimer, pauseState.timeOverLeft);
        setTimeLeft(m_autoPlayTimer, pauseState.autoPlayLeft);
        pauseState.mediaPosition = m_mediaPlayer->position();
        m_mediaPlayer->stop();

        // initialize dialog and set its colors
        PauseDialog dlg(this, QColor(BOARD_TEXT), OptionsData::GetInstance());

        auto dialogReturnCode = dlg.exec();

        // we update the options if the user changes them
        // regardless if they quit the game or continued
        if( dlg.HaveOptionsChanged() )
        {
            OptionsData::GetInstance() = dlg.GetOptions();
            SetOptions(OptionsData::GetInstance());
        }

        if( dialogReturnCode == QDialog::Accepted)
        {
            m_mode = originalMode;

            // restart any timers that had time left
            if( m_mode == FINAL_CLUE)
            {
                m_mediaPlayer->setPosition(pauseState.mediaPosition);
                m_mediaPlayer->play();
            }

            if( pauseState.clueTimeLeft > 0)
            {
                StartClueTimer(pauseState.clueTimeLeft);
            }

            if( pauseState.timeOverLeft > 0)
            {
                StartTimeOverTimer(pauseState.timeOverLeft);
            }

            if( pauseState.autoPlayLeft > 0)
            {
                StartAutoPlayTimer();
            }
        }
        else
        {
            // gracefully quit the game and return to main screen
            m_mode = MENU;
            emit GameOver();
        }
    }
}

void
GamePaneWidget::StartClueTimer( const unsigned int milliSeconds)
{
    m_clueTimer = new QTimer(this);
    m_clueTimer->setInterval(milliSeconds);
    m_clueTimer->setSingleShot(true);
    connect(m_clueTimer, &QTimer::timeout, this, &GamePaneWidget::OnClueTimerOut);
    m_clueTimer->start();
}

void
GamePaneWidget::UpdateMediaPlayerFromOptions()
{
    m_mediaPlayer->setVolume(m_options.m_music.volume);
    m_mediaPlayer->setMuted(!m_options.m_music.playFinalJeopardy);
}

GamePaneWidget::~GamePaneWidget() {}
