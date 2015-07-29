#include "jeopardygame.h"
#include <QTime>
#include <QDebug>

using GameStateUtils::TOTAL_COLS;
using GameStateUtils::TOTAL_ROWS;


JeopardyGame::JeopardyGame()
    : JeopardyGame(OptionsData::GetInstance().m_nextClueOptions)
{
}

JeopardyGame::JeopardyGame(NextClueOptions& nextClueOptions)
    : m_gameMode(GM_NONE)
    , m_cluesAnswered(0)
    , m_nextClueOptions(nextClueOptions)
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
}

JeopardyGame::~JeopardyGame()
{
}

const GameStateUtils::Clues&
JeopardyGame::GetCurrentClues() const
{
    return m_jeopardyGameInfo.currentClues;
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
    DatabaseUtils::GetJeopardyGameInfo(gameID, m_jeopardyGameInfo/*out*/);

    LoadRound(GM_SINGLE);
}

void
JeopardyGame::LoadRound( const GameMode gameMode )
{
    if( gameMode != GM_SINGLE && gameMode != GM_DOUBLE )
        return;

    m_jeopardyGameInfo.currentClues = (gameMode == GM_SINGLE) ? m_jeopardyGameInfo.singleRoundClues : m_jeopardyGameInfo.doubleRoundClues;

    m_cluesAnswered = 0;
    m_gameMode = gameMode;
}

/*
 *  User clicked on a clue on the board - return clue's question
 *
 *  Returning an empty string denotes to ignore this action
*/
QString
JeopardyGame::HandleBoardAction(const int column, const int row)
{
    // This will set answered to true, if it hasn't been answered yet
    return m_jeopardyGameInfo.currentClues.GetClueQuestion(column, row);
}

/*
 *  User clicked on a clue - return clue's answer
 *
 *  Returning an empty string denotes to ignore this action
 */
QString
JeopardyGame::HandleClueAction(const int column, const int row)
{
    // clue has been answered!
    m_cluesAnswered++;

    return m_jeopardyGameInfo.currentClues.GetClueAnswer(column, row);
}

/*
 *  User clicked on index's answer - Handle changing rounds here.
 */
JeopardyGame::GameMode
JeopardyGame::HandleAnswerAction()
{
    switch( m_gameMode )
    {
        case GM_SINGLE:
            if( m_cluesAnswered == m_jeopardyGameInfo.totalSingleClues)
            {
                LoadRound(GM_DOUBLE);
                return GM_DOUBLE;
            }
            break;

        case GM_DOUBLE:
            if( m_cluesAnswered == m_jeopardyGameInfo.totalDoubleClues)
            {
                m_gameMode = GM_FINAL;

                return GM_FINAL;
            }
            break;

        default:
            break;
    }

    return GM_NONE;
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
}

bool
JeopardyGame::IsClueAvailable(const int column, const int row) const
{
    return !m_jeopardyGameInfo.currentClues.HasClueBeenAnswered(column, row);
}

std::pair<int,int>
JeopardyGame::GetNextClue(const int currentColumn, const int currentRow)
{
    /**
     * This is the algorithm
     *
     * if currCol.IsEmpty() || 90% chance
     *      newCol = new equally random column
     *
     * if newCol != currCol
     *      newRow = 70% chance ? lowest available ? random avail
     * else
     *      newRow = 90% chance ? next lowest available ? random avail
     **/

    int newColumn = currentColumn;
    bool isCurrColumnEmpty = true;
    for( int row = 0; row< TOTAL_ROWS; row++)
    {
        if(IsClueAvailable(currentColumn, row))
        {
            isCurrColumnEmpty = false;
            break;
        }
    }

    if( isCurrColumnEmpty || IsWithinRandPercentage(m_nextClueOptions.NewColumnChance) )
    {
        std::vector<int> availCols;
        for( int col = 0; col<TOTAL_COLS; col++)
        {
            if( col == currentColumn)
                continue;

            for( int row = 0; row<TOTAL_ROWS; row++)
            {
                if(IsClueAvailable(col, row))
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
        if( IsClueAvailable(newColumn, row))
        {
            availRows.push_back(row);
        }
    }

    Q_ASSERT(!availRows.empty());

    if( newColumn != currentColumn)
    {
        if( IsWithinRandPercentage(m_nextClueOptions.NextRowNewColumnChance))
        {
            newRow = availRows[0];
        }
        else
        {
            newRow = availRows[GetRandomIndex(availRows.size())];
        }
    }
    else    // Not changing columns
    {
        const bool useNextLowest = IsWithinRandPercentage(m_nextClueOptions.NextRowSameColumnChance);

        if( useNextLowest)
        {
            // current clue has been answered ( unless it's the start of the game
            // and new clues have been answered yet ), so start at the next row
            const int startRow = currentRow + (m_cluesAnswered == 0 ? 0 : 1);

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

    return std::make_pair(newRow,newColumn);
}

const QString&
JeopardyGame::GetFinalCategory() const
{
    return m_jeopardyGameInfo.finalCategory;
}

const QString&
JeopardyGame::GetFinalClue() const
{
    return m_jeopardyGameInfo.finalClue;
}

const QString&
JeopardyGame::GetFinalAnswer() const
{
    return m_jeopardyGameInfo.finalAnswer;
}

void
JeopardyGame::SetNextClueOptions(const NextClueOptions& nextClueOptions)
{
    m_nextClueOptions = nextClueOptions;
}
