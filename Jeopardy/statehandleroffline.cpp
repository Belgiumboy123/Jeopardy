#include "statehandleroffline.h"

#include "jeopardygame.h"
#include "qtutility.h"

#include <QStandardItemModel>

using GameStateUtils::TOTAL_COLS;
using GameStateUtils::TOTAL_ROWS;
using GameStateUtils::GameState;

StateHandlerOffline::StateHandlerOffline()
    : IStateHandler()
    , m_game(new JeopardyGame)
{
}

StateHandlerOffline::~StateHandlerOffline() {}

QStandardItemModel*
StateHandlerOffline::GetModel() const
{
    return m_model.get();
}

void
StateHandlerOffline::LoadModelFromCurrentClues()
{
    const auto& clues = m_game->GetCurrentClues();

    m_model->clear();

    for( int column = 0; column<TOTAL_COLS; column++)
    {
        // add header item for category
        QStandardItem* horizontalItem = new QStandardItem(clues.GetCategoryHeader(column));
        m_model->setHorizontalHeaderItem(column, horizontalItem);

        for( int row = 0; row<TOTAL_ROWS; row++)
        {
            QStandardItem* item = new QStandardItem;
            item->setText(clues.GetClueText(column, row));
            item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            item->setForeground(QBrush(BOARD_TEXT));
            item->setBackground(QBrush(CLUE_BLUE));

            m_model->setItem(row, column, item);
        }
    }
}

void
StateHandlerOffline::SetNextClueOptions(const NextClueOptions& nextClueOptions)
{
    m_game->SetNextClueOptions(nextClueOptions);
}

void
StateHandlerOffline::DoActionOnState(GameStateUtils::GameState currentState)
{
    DoActionOnState(currentState, QModelIndex());
}

void
StateHandlerOffline::DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index)
{

    switch(currentState)
    {
    case GameState::MENU:
        m_game->LoadRandomGame();
        LoadModelFromCurrentClues();
        emit StateChanged(GameState::BOARD_START, GetModel()->index(0,0));
        break;

    case GameState::BOARD:
    {
        const QString& clue = m_game->HandleBoardAction(index.column(), index.row());

        // Only emit state changed if clue isn't empty.
        // The clue might be empty because the either the clue has already been asked
        // Or the clue never existed to begin with
        if(!clue.isEmpty())
        {
            GetModel()->itemFromIndex(index)->setText("");
            emit StateChanged(GameState::CLUE_QUESTION, index, clue);
        }
        break;
    }

    case GameState::CLUE_QUESTION:
    {
        const QString& answer = m_game->HandleClueAction(index.column(), index.row());
        emit StateChanged(GameState::CLUE_ANSWER, index, answer);
        break;
    }

    case GameState::CLUE_ANSWER:
    {
        auto newMode = m_game->HandleAnswerAction();
        if( newMode == JeopardyGame::GM_FINAL )
        {
            emit StateChanged(GameState::FINAL_START);
        }
        else if( newMode == JeopardyGame::GM_DOUBLE)
        {
            LoadModelFromCurrentClues();
            emit StateChanged(GameState::BOARD_START, GetModel()->index(0,0));
        }
        else
        {
            const auto& pair = m_game->GetNextClue(index.column(), index.row());
            emit StateChanged(GameState::BOARD, GetModel()->index(pair.first, pair.second));
        }
        break;
    }

    case GameState::FINAL_START:
        emit StateChanged(GameState::FINAL_CATEGORY, QModelIndex(), m_game->GetFinalCategory());
        break;

     case GameState::FINAL_CATEGORY:
        emit StateChanged(GameState::FINAL_CLUE, QModelIndex(), m_game->GetFinalClue());
        break;

     case GameState::FINAL_CLUE:
        emit StateChanged(GameState::FINAL_ANSWER, QModelIndex(), m_game->GetFinalAnswer());
        break;

     case GameState::FINAL_ANSWER:
        emit StateChanged(GameState::GAME_OVER);
        break;

    default:
        break;
    }
}

