#include "jeopardygame.h"

#include <QStandardItemModel>
#include <QTime>
#include <QDebug>

#define CLUE_BLUE "#0A06B3"
#define BOARD_TEXT "#E29D44"

namespace
{
    const QString DOLLAR = "$";

    const int TOTAL_ROWS = 5;
    const int TOTAL_COLS = 6;
}

class JeopardyItem : public QStandardItem
{
public:

    JeopardyItem(const QString& clue, const QString& answer)
        : QStandardItem()
        , m_clue(clue)
        , m_answer(answer)
    {
        setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        setForeground(QBrush(BOARD_TEXT));
        setBackground(QBrush(CLUE_BLUE));
    }

    void SetClue(const QString& s){ m_clue = s; }
    QString GetClue() const { return m_clue; }

    void SetAnswer(const QString& s){ m_answer = s; }
    QString GetAnswer() const { return m_answer; }

private:

    QString m_clue;
    QString m_answer;
};

JeopardyGame::JeopardyGame()
    : m_model( new QStandardItemModel(TOTAL_ROWS, TOTAL_COLS))
    , m_gameMode(GM_NONE)
    , m_cluesAnswered(0)
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
}

JeopardyGame::~JeopardyGame()
{
    if( m_model)
        delete m_model;
}

QStandardItemModel*
JeopardyGame::GetModel() const
{
    return m_model;
}

/**
 * Loads a random game that hasn't been played before
 *
 * Once a game has been laoded we immediately mark it as played
 */
void
JeopardyGame::LoadRandomGame()
{
    const int gameID  = DatabaseUtils::GetANonPlayedGame();
    LoadGame(gameID);
}

void
JeopardyGame::LoadGame(const int gameID )
{
    DatabaseUtils::GetGameInfo(gameID, m_staticGameInfo /*out*/);

    LoadRound(GM_DOUBLE);
}

void
JeopardyGame::LoadRound( const GameMode gameMode )
{
    if( gameMode != GM_SINGLE && gameMode != GM_DOUBLE )
        return;

    const DatabaseUtils::RoundQuestions& roundQuestions = (gameMode == GM_SINGLE) ? m_staticGameInfo.singleRoundQuestions
                                                                                  : m_staticGameInfo.doubleRoundQuestions;

    m_model->clear();

    int column = 0;
    for( const auto& category : roundQuestions)
    {
        // add header item for category
        QStandardItem* horizontalItem = new QStandardItem(category.first);
        m_model->setHorizontalHeaderItem(column, horizontalItem);

        for( const auto& clue : category.second)
        {
            JeopardyItem* item = new JeopardyItem( clue.second.first, clue.second.second );
            if(!clue.second.first.isEmpty())
            {
                item->setText( DOLLAR + QString::number(clue.first) );
            }
            m_model->setItem(GetRowFromValue(clue.first, gameMode), column, item);
        }

        //Set empty items for missing empty clues
        if( category.second.size() != TOTAL_ROWS)
        {
            for( int i = 0; i<TOTAL_ROWS; i++)
            {
                if( !m_model->item(i,column))
                {
                    JeopardyItem* item = new JeopardyItem( "", "" );
                    m_model->setItem(i, column, item);
                }
            }
        }

        column++;
    }

    m_cluesAnswered = 0;
    m_gameMode = gameMode;
}

/*
 *  User clicked on a clue on the board - return clue's question
 *
 *  Returning an empty string denotes to ignore this action
*/
QString
JeopardyGame::HandleBoardAction( const QModelIndex& index )
{
    if( !index.isValid() )
        return "";

    // static_cast cause we know, use dynamic_cast, if separate category item class
    JeopardyItem* item = static_cast<JeopardyItem*>(m_model->itemFromIndex(index));

    if( !item || item->text().isEmpty() )
        return "";

    // clue has been clicked so, it's empty it's text
    item->setText("");

    return item->GetClue();
}

/*
 *  User clicked on a clue - return clue's answer
 *
 *  Returning an empty string denotes to ignore this action
 */
QString
JeopardyGame::HandleClueAction( const QModelIndex& index )
{
    if( !index.isValid() )
        return "";

    JeopardyItem* item = static_cast<JeopardyItem*>(m_model->itemFromIndex(index));

    if( !item )
        return "";

    // clue has been answered!
    m_cluesAnswered++;

    return item->GetAnswer();
}

/*
 *  User clicked on index's answer - Handle changing rounds here.
 */
bool
JeopardyGame::HandleAnswerAction()
{
    switch( m_gameMode )
    {
        case GM_SINGLE:
            if( m_cluesAnswered == m_staticGameInfo.totalSingleClues)
            {
                LoadRound(GM_DOUBLE);
            }
            break;

        case GM_DOUBLE:
            if( m_cluesAnswered == m_staticGameInfo.totalDoubleClues)
            {
                m_gameMode = GM_FINAL;

                // return true when FINAL round has begun
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

namespace
{
    bool IsWithinRandPercentage(int percent)
    {
        return qrand() % 101 < percent;
    }

    int GetRandomIndex(int size)
    {
        return qrand() % size;
    }

    bool IsItemAvailable(const QStandardItemModel* model, const int row, const int col)
    {
        if( !model->item(row,col))
        {
            qDebug() << "ERROR " << row  << " " << col;
            return false;
        }

        return !model->item(row,col)->text().isEmpty();
    }
}

QModelIndex
JeopardyGame::GetNextClue(const QModelIndex& currentClue)
{
    /**
     * This is the algorithm
     *
     * if currCol.IsEmpty() || 90% chance
     *      newCol = new equally random column
     *
     * if newCol != currCol
     *      newRow = 75% chance ? lowest available ? random avail
     * else
     *      newRow = 90% chance ? next lowest available ? random avail
     **/

    const int currColumn = currentClue.column();
    int newColumn = currColumn;
    bool isCurrColumnEmpty = true;
    for( int row = 0; row< TOTAL_ROWS; row++)
    {
        if(IsItemAvailable(m_model,row,currColumn))
        {
            isCurrColumnEmpty = false;
            break;
        }
    }

    if( isCurrColumnEmpty || IsWithinRandPercentage(10))
    {
        std::vector<int> availCols;
        for( int col = 0; col<TOTAL_COLS; col++)
        {
            if( col == currColumn)
                continue;

            for( int row = 0; row<TOTAL_ROWS; row++)
            {
                if(IsItemAvailable(m_model,row,col))
                {
                    availCols.push_back(col);
                    break;
                }
            }
        }

        if(availCols.size())
        {
            newColumn = availCols[GetRandomIndex(availCols.size())];
        }
    }

    // at this point we are guaranteed that newColumn contains valid rows
    // otherwise it should not have been chosen by the above code

    int newRow = -1;
    std::vector<int> availRows;
    for( int row = 0; row <TOTAL_ROWS; row++)
    {
        if( IsItemAvailable(m_model,row,newColumn))
        {
            availRows.push_back(row);
        }
    }

    // assert that availRows is not empty

    if( newColumn != currColumn)
    {
        if( IsWithinRandPercentage(70))
        {
            newRow = availRows[0];
        }
        else
        {
            newRow = availRows[GetRandomIndex(availRows.size())];
        }
    }
    else
    {
        const bool useNextLowest = IsWithinRandPercentage(90);

        if( useNextLowest)
        {
            // current clue has been answered ( unless it's the start of the game
            // and new clues have been answered yet ), so start at the next row
            const int startRow = currentClue.row() + (m_cluesAnswered == 0 ? 0 : 1);

            for( auto row : availRows )
            {
                if( row >= startRow )
                {
                    newRow = row;
                    break;
                }
            }
        }

        if( newRow == -1 )
        {
            newRow = availRows[GetRandomIndex(availRows.size())];
        }
    }

    return m_model->index(newRow,newColumn);
}

const QString&
JeopardyGame::GetFinalCategory() const
{
    return m_staticGameInfo.finalCategory;
}

const QString&
JeopardyGame::GetFinalClue() const
{
    return m_staticGameInfo.finalClue;
}

const QString&
JeopardyGame::GetFinalAnswer() const
{
    return m_staticGameInfo.finalAnswer;
}

int
JeopardyGame::GetRowFromValue( const int value, const GameMode mode) const
{
    return (value / (mode == GM_DOUBLE ? 400 : 200) ) - 1;
}
