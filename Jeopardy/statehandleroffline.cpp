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

void
StateHandlerOffline::LoadModelFromCurrentClues(GameStateUtils::Clues* clues)
{
    if(!clues)
    {   // Theoretically we should never hit this case
        return;
    }

    m_model->clear();

    for( int column = 0; column<TOTAL_COLS; column++)
    {
        // add header item for category
        QStandardItem* horizontalItem = new QStandardItem(clues->GetCategoryHeader(column));
        m_model->setHorizontalHeaderItem(column, horizontalItem);

        for( int row = 0; row<TOTAL_ROWS; row++)
        {
            QStandardItem* item = new QStandardItem;
            item->setText(clues->GetClueText(column, row));
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
StateHandlerOffline::DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index)
{
    GameStateUtils::StateAction action;
    action.column = index.column();
    action.row = index.row();
    action.state = currentState;

    const auto& response = m_game->DoStateAction(action);
    const auto& responseIndex = GetModel()->index(response.row, response.column);

    // Handle some special response cases.
    switch(response.state)
    {
        case GameState::BOARD_START:
            LoadModelFromCurrentClues(response.clues);
            break;

        case GameState::CLUE_QUESTION:
            // Only emit state changed if clue isn't empty.
            // The clue might be empty because the either the clue has already been asked
            // Or the clue never existed to begin with
            if(response.message.isEmpty())
            {
                return;
            }

            GetModel()->itemFromIndex(responseIndex)->setText("");
            break;

        default:
            break;
    }

    emit StateChanged(response.state, responseIndex, response.message);
}

