#include "battleshipstatehandleronline.h"

BattleshipStateHandlerOnline::BattleshipStateHandlerOnline()
    : IStateHandlerOnline()
{
}

BattleshipStateHandlerOnline::~BattleshipStateHandlerOnline()
{
}


void
BattleshipStateHandlerOnline::DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index /*=QModelIndex()*/)
{
    IStateHandlerOnline::DoActionOnState(currentState, index);
}

void
BattleshipStateHandlerOnline::HandleServerResponse(const GameStateUtils::StateResponse& response)
{
    // $Mark use this as chat in when the game has started
    emit ConnectionMessage(response.message);

    //emit StateChanged(response.state, responseIndex, response.message);
}
