#pragma once

#include <QObject>

#include "gamestateutils.h"

class JeopardyGame;
class QTcpServer;
class QTcpSocket;

class JeopardyServer : public QObject
{
    Q_OBJECT

public:
    JeopardyServer();
    virtual ~JeopardyServer();

    std::pair<bool,QString> StartServer(const int port);
    void CloseServer();

signals:
    void ServerMessage(const QString& message);

private:
    void OnNewConnection();
    void OnClientMessage();

    std::unique_ptr<JeopardyGame> m_game;

    // Game state the server is currently in.
    // This is recorded to help synchronize between
    // clients
    GameStateUtils::GameState m_serverGameState;

    QTcpServer* m_server;

    QList<QTcpSocket*> m_sockets;
};

