#include "jeopardystatehandleronline.h"

#include "qtutility.h"

#include <QStandardItemModel>

using GameStateUtils::TOTAL_COLS;
using GameStateUtils::TOTAL_ROWS;
using GameStateUtils::GameState;

JeopardyStateHandlerOnline::JeopardyStateHandlerOnline()
    : IStateHandlerOnline()
{
}

JeopardyStateHandlerOnline::~JeopardyStateHandlerOnline() {}

void
JeopardyStateHandlerOnline::LoadModelFromCluesString(QString clues)
{
    if( clues.isEmpty())
    {
        return;
    }

    m_model->clear();

    bool parseSuccess = true;

    for( int column = 0; column<TOTAL_COLS; column++)
    {
        // add header item for category
        auto result = GameStateUtils::GetCategoryHeader(clues);
        if(!result.first) parseSuccess = false;

        QStandardItem* horizontalItem = new QStandardItem(result.second);
        m_model->setHorizontalHeaderItem(column, horizontalItem);

        for( int row = 0; row<TOTAL_ROWS; row++)
        {
            auto result = GameStateUtils::GetClueText(clues);
            if(!result.first) parseSuccess = false;

            QStandardItem* item = new QStandardItem;
            item->setText(result.second);
            item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            item->setForeground(QBrush(BOARD_TEXT));
            item->setBackground(QBrush(CLUE_BLUE));

            m_model->setItem(row, column, item);
        }
    }

    if( !parseSuccess)
    {
        m_model->clear();
    }
}

void
JeopardyStateHandlerOnline::DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index /*=QModelIndex()*/)
{
    if( currentState == GameState::MENU)
    {
        // Server loaded the board on state SERVER_START_GAME
        // so just return board start here.

        // We could either fire another message to this to server
        // Though this seems a bit redundant, or save and use
        // the responseIndex from the SERVER_START_Game state

        const auto& responseIndex = GetModel()->index(0, 0);
        emit StateChanged(GameState::BOARD_START, responseIndex, "");
        return;
    }

    IStateHandlerOnline::DoActionOnState(currentState, index);
}

void
JeopardyStateHandlerOnline::HandleServerResponse()
{
    /*
    auto message = m_socket->readAll();
    QString str = QString(message.constData());
    auto pair = GameStateUtils::StateResponse::GenerateFromString(str);

    if(pair.first)
    {
        const GameStateUtils::StateResponse& response = pair.second;
        const auto& responseIndex = GetModel()->index(response.row, response.column);

        // $Mark use this as chat in when the game has started
        emit ConnectionMessage(response.message);

        switch(response.state)
        {
        case GameState::SERVER_START_MENU:
            emit BothPlayersConnected();
            return;

        case GameState::SERVER_GAME_START:
            LoadModelFromCluesString(response.serverClues);
            emit StartGame();
            return;

        case GameState::BOARD_START:
            LoadModelFromCluesString(response.serverClues);
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

        case GameState::OPPONENT_DISCONNECTED:
            emit ConnectionLost("Opponent has disconnected!");
            break;

        default:
            break;
        }

        emit StateChanged(response.state, responseIndex, response.message);
    }
    */
}

