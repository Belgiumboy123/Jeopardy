#pragma once

#include "istatehandleronline.h"

class BattleshipStateHandlerOnline : public IStateHandlerOnline
{
public:
    BattleshipStateHandlerOnline();
    virtual ~BattleshipStateHandlerOnline();

    virtual void DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index = QModelIndex());

private:
    virtual void HandleServerResponse(const GameStateUtils::StateResponse& response);
};

