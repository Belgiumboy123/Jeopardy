#include "statehandleroffline.h"

#include "jeopardygame.h"

#include <QStandardItemModel>

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
    return m_game->GetModel();
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
        emit StateChanged(GameState::BOARD_START, m_game->GetModel()->index(0,0));
        break;

    case GameState::BOARD:
    {
        const QString& clue = m_game->HandleBoardAction(index);

        // Only emit state changed if clue isn't empty.
        // The clue might be empty because the either the clue has already been asked
        // Or the clue never existed to begin with
        if(!clue.isEmpty())
        {
            emit StateChanged(GameState::CLUE_QUESTION, index, clue);
        }
        break;
    }

    case GameState::CLUE_QUESTION:
    {
        const QString& answer = m_game->HandleClueAction(index);
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
            emit StateChanged(GameState::BOARD_START, GetModel()->index(0,0));
        }
        else
        {
            emit StateChanged(GameState::BOARD, m_game->GetNextClue(index));
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

