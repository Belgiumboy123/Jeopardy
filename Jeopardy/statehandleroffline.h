#pragma once

#include "istatehandler.h"

class JeopardyGame;

class StateHandlerOffline : public IStateHandler
{
    //Q_OBJECT

public:
    StateHandlerOffline();
    virtual ~StateHandlerOffline();

    virtual void DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index);

    virtual void SetNextClueOptions(const NextClueOptions& nextClueOptions);

private:
    void LoadModelFromCurrentClues(GameStateUtils::Clues* clues);

    std::unique_ptr<JeopardyGame> m_game;
};

