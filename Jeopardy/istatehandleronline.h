#pragma once

#include "istatehandler.h"

#include <QTcpSocket>

class IStateHandlerOnline : public IStateHandler
{
    Q_OBJECT

public:
    IStateHandlerOnline();
    virtual ~IStateHandlerOnline();

    virtual void SetNextClueOptions(const NextClueOptions& nextClueOptions);
    virtual bool AllowUserInteraction() const;

    void ConnectToHost(const QString& hostname, const int port);

    virtual void DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index = QModelIndex());

signals:
    void ConnectionMessage(const QString& message);
    void ConnectionMade();
    void ConnectionLost(const QString& message);
    void BothPlayersConnected();
    void StartGame();

private:
    void OnHostFound();
    void OnSocketError(QAbstractSocket::SocketError socketError);
    void OnSocketConnected();
    void OnStateChanged(QAbstractSocket::SocketState socketState);
    void OnDisconnected();
    void OnServerMessage();

    virtual void HandleServerResponse(const GameStateUtils::StateResponse& response) = 0;

    QTcpSocket* m_socket;
};

