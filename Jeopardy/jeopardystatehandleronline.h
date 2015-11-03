#pragma once

#include "istatehandleronline.h"

class JeopardyStateHandlerOnline : public IStateHandlerOnline
{
    Q_OBJECT

public:
    JeopardyStateHandlerOnline();
    virtual ~JeopardyStateHandlerOnline();

    virtual void DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index = QModelIndex());

private:
    virtual void HandleServerResponse(const GameStateUtils::StateResponse& response);

    void LoadModelFromCluesString(QString clues);
};

