#pragma once

#include <QObject>
#include <QModelIndex>

#include "gamestateutils.h"

struct NextClueOptions;
class QStandardItemModel;
class QString;

class IStateHandler : public QObject
{
    Q_OBJECT

public:
    explicit IStateHandler();
    virtual ~IStateHandler();

    virtual void DoActionOnState(GameStateUtils::GameState currentState);
    virtual void DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index) = 0;

    virtual QStandardItemModel* GetModel() const;

    virtual void SetNextClueOptions(const NextClueOptions& nextClueOptions) = 0;
    virtual bool AllowUserInteraction() const = 0;

signals:
    void StateChanged(GameStateUtils::GameState state, const QModelIndex& index = QModelIndex(), const QString& message = "");

protected:
    std::shared_ptr<QStandardItemModel> m_model;
};

