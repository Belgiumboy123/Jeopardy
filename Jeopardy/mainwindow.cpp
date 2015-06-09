#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jeopardygame.h"

#include <QItemDelegate>
#include <QKeyEvent>
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
        QStyleOptionViewItem opt = option;
        opt.rect = option.rect.adjusted(3, 3, -3, -3);
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
    , m_game( new JeopardyGame )
{
    m_ui->setupUi(this);

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

    auto pickGamePal = m_ui->pickGameLabel->palette();
    pickGamePal.setColor(m_ui->pickGameLabel->foregroundRole(), BOARD_TEXT);
    m_ui->pickGameLabel->setPalette(pickGamePal);
    boardFont.setPointSize(200);
    m_ui->pickGameLabel->setFont(boardFont);

    clueFont.setPointSize(40);
    m_ui->pickGameButton->setFont(clueFont);

    m_ui->pickGameWidget->setAutoFillBackground(true);
    m_ui->pickGameWidget->setPalette(cluePal);

    connect( m_ui->pickGameButton, &QPushButton::clicked, this, &MainWindow::handleStartGameClick );

    clueFont.setPointSize(28);
    m_ui->autoPlayCheckBox->setFont(clueFont);
    connect( m_ui->autoPlayCheckBox, &QCheckBox::toggled,
             m_ui->autoPlayOptionsButton, &QWidget::setVisible );

    clueFont.setPointSize(14);
    m_ui->autoPlayOptionsButton->setFont(clueFont);
    connect( m_ui->autoPlayOptionsButton, &QPushButton::clicked, this, &MainWindow::launchAutoPlayOptionsDialog );

    // NOTE setting the fixed size with both autoplay widgets visible,
    // ensures that the widget does not change size when the auto play
    // checkbox is checked. Thus order of the following two lines is important.
    m_ui->autoPlayWidget->setFixedSize(m_ui->autoPlayWidget->sizeHint());
    m_ui->autoPlayOptionsButton->hide();

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

    if( m_mode == CLUE_QUESTION)
    {
        if( m_timeOverTimer && m_timeOverTimer->isActive())
        {
            m_timeOverTimer->stop();
        }

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
        const bool startFinal = m_game->HandleAnswerAction();

        if( startFinal )
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
            m_ui->tableView->selectionModel()->setCurrentIndex(m_clickedIndex, QItemSelectionModel::NoUpdate);
            m_ui->tableView->clearSelection();
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

        // show final jeopardy category and for 7 seconds /  optional click;
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
    }
    else if( m_mode == FINAL_CLUE)
    {
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
        m_ui->clueWidget->hide();
        m_ui->pickGameWidget->show();
    }
}

void
MainWindow::AutoPlayNextClue()
{
    m_autoPlayState.newIndex = m_game->GetNextClue();
    m_autoPlayState.currColumn = m_clickedIndex.column();
    m_autoPlayState.currRow = m_clickedIndex.row();
    m_autoPlayState.columnDirection = m_autoPlayState.currColumn < m_autoPlayState.newIndex.column() ? 1 : -1;
    m_autoPlayState.rowDirection = m_autoPlayState.currRow < m_autoPlayState.newIndex.row() ? 1 : -1;

    // change the mode so we can correctly handle board clicks
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
            }

            if(keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
            {
                if( m_mode == BOARD)
                {
                    handleBoardClick(m_ui->tableView->currentIndex());
                }
                else if( m_mode != CLUE_ANIMATION)
                {
                    handleClueClick();
                }

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
MainWindow::launchAutoPlayOptionsDialog()
{
    AutoPlayOptionsDialog dlg;
    dlg.SetTimeIntervals(m_timeIntervals);
    if(dlg.exec() == QDialog::Accepted)
    {
        m_timeIntervals = dlg.GetTimeIntervals();
    }
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
