#pragma once

#include <QObject>

#include "gamestateutils.h"

class JeopardyGame;
class QTcpServer;
class QTcpSocket;

enum class JeopardyServerMessageType
{
    SENT = 0,
    RECEIVED,
    IGNORED,
    NONE,
    ERROR
};

class JeopardyServer : public QObject
{
    Q_OBJECT

public:
    JeopardyServer();
    virtual ~JeopardyServer();

    std::pair<bool,QString> StartServer(const int port);
    void CloseServer();

signals:
    void ServerMessage(const QString& message, JeopardyServerMessageType type);

private:
    void OnClientDisconnected();
    void OnNewConnection();
    void OnClientMessage();
    void SendResponseToClients(const GameStateUtils::StateResponse& response);

    std::unique_ptr<JeopardyGame> m_game;

    // Game state the server is currently in.
    // This is recorded to help synchronize between
    // clients
    GameStateUtils::GameState m_serverGameState;

    QTcpServer* m_server;

    QList<QTcpSocket*> m_sockets;
    QList<QTcpSocket*> m_jeopardySockets;

    enum Games
    {
        Jeopardy = 0,
        Battleship,
        NUM_GAMES
    };

    std::vector<int> m_gamePlayersReady;

    int m_playersReadyToPlay;
};

