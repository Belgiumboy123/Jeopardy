#include "istatehandler.h"

#include <QStandardItemModel>

using GameStateUtils::TOTAL_COLS;
using GameStateUtils::TOTAL_ROWS;

IStateHandler::IStateHandler()
    : QObject(nullptr)
    , m_model(std::make_shared<QStandardItemModel>(TOTAL_ROWS, TOTAL_COLS))
{
}

IStateHandler::~IStateHandler() {}

QStandardItemModel*
IStateHandler::GetModel() const
{
    return m_model.get();
}

void
IStateHandler::DoActionOnState(GameStateUtils::GameState currentState)
{
    DoActionOnState(currentState, QModelIndex());
}

