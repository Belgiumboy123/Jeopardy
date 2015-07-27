#pragma once

#include "istatehandler.h"

class JeopardyGame;

class StateHandlerOffline : public IStateHandler
{
public:
    StateHandlerOffline();
    virtual ~StateHandlerOffline();

    virtual void DoActionOnState(GameStateUtils::GameState currentState);
    virtual void DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index);

    virtual QStandardItemModel* GetModel() const;

    virtual void SetNextClueOptions(const NextClueOptions& nextClueOptions);

private:
    std::unique_ptr<JeopardyGame> m_game;
};
