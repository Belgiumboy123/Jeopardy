#pragma once

#include "istatehandler.h"

#include <QTcpSocket>

class StateHandlerOnline : public IStateHandler
{
    Q_OBJECT

public:
    StateHandlerOnline();
    virtual ~StateHandlerOnline();

    virtual void DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index);
    virtual void SetNextClueOptions(const NextClueOptions& nextClueOptions);

    void ConnectToHost(const QString& hostname, const int port);

signals:
    void ConnectionMessage(const QString& message);
    void ConnectionMade();
    void ConnectionLost(const QString& message);

private:
    void OnHostFound();
    void OnSocketError(QAbstractSocket::SocketError socketError);
    void OnSocketConnected();
    void OnStateChanged(QAbstractSocket::SocketState socketState);
    void OnDisconnected();
    void OnServerMessage();

    QTcpSocket* m_socket;
};

