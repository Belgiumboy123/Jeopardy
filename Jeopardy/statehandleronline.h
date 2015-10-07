#pragma once

#include "istatehandler.h"

#include <QTcpSocket>

class StateHandlerOnline : public IStateHandler
{
    Q_OBJECT

public:
    StateHandlerOnline();
    virtual ~StateHandlerOnline();

    virtual void DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index = QModelIndex());
    virtual void SetNextClueOptions(const NextClueOptions& nextClueOptions);
    bool AllowUserInteraction() const;

    void ConnectToHost(const QString& hostname, const int port);

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

    void LoadModelFromCluesString(QString clues);

    QTcpSocket* m_socket;
};

