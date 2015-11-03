#include "jeopardyserver.h"

#include "jeopardygame.h"

#include <QNetworkInterface>
#include <QTcpSocket>
#include <QTcpServer>

using GameStateUtils::GameState;
using GameStateUtils::GameStateString;
using JeopardyServerMessageType::NONE;
using JeopardyServerMessageType::SENT;
using JeopardyServerMessageType::IGNORED;
using JeopardyServerMessageType::RECEIVED;
using JeopardyServerMessageType::ERROR;

JeopardyServer::JeopardyServer()
    : QObject()
    , m_playersReadyToPlay(0)
{
    // For now JeopardyServer is loaded with the local system's
    // saved game options. It should be loading and saving its own
    // options.

    m_game.reset(new JeopardyGame);

    m_serverGameState = GameState::SERVER_OFFLINE;

    m_gamePlayersReady.assign(NUM_GAMES, 0);

    m_server = new QTcpServer(this);
}

std::pair<bool,QString>
JeopardyServer::StartServer(const int port)
{
    auto pair = std::make_pair(false, tr(""));

    QHostAddress address(QHostAddress::Any);
    if( !m_server->listen(address, port) )
    {
        pair.second = tr("Unable to start the server: %1.").arg(m_server->errorString());
    }
    else    // Success!
    {
        m_serverGameState = GameState::SERVER_ONLINE;
        m_server->resumeAccepting();

        QString ipAddress;
        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
        // use the first non-localhost IPv4 address
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
                ipAddress = ipAddressesList.at(i).toString();
                break;
            }
        }
        if (ipAddress.isEmpty())
            ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

        pair.first = true;
        pair.second = tr("Server started on IP: %1 on host %2.").arg(ipAddress).arg(QString::number(port));

        emit ServerMessage("Server started.", NONE);
        connect( m_server, &QTcpServer::newConnection, this, &JeopardyServer::OnNewConnection);
    }

    return pair;
}

void
JeopardyServer::OnNewConnection()
{

    QTcpSocket* socket = m_server->nextPendingConnection();
    if(!socket)
        return;

    connect( socket, &QAbstractSocket::disconnected, this, &JeopardyServer::OnClientDisconnected);

    m_sockets << socket;

    emit ServerMessage( QString::number(socket->socketDescriptor()) + tr(": made Connection."), NONE );

    connect( socket, &QTcpSocket::readyRead, this, &JeopardyServer::OnClientMessage );

    // Tell the client to pick a game
    GameStateUtils::StateResponse response;
    response.state = GameState::SERVER_GAME_PICK;
    auto message = response.ToString().toLocal8Bit();
    emit ServerMessage(tr("Sending message to new player: ") + message, SENT);
    socket->write( message );
}

void
JeopardyServer::OnClientDisconnected()
{
    QTcpSocket* disconnectedSocket = qobject_cast<QTcpSocket*>(QObject::sender());

    for( auto socket : m_sockets)
    {
        if( socket != disconnectedSocket)
        {
            GameStateUtils::StateResponse response;
            response.state = GameState::OPPONENT_DISCONNECTED;

            const auto message = response.ToString().toLocal8Bit();
            socket->write( message );
        }

        socket->disconnectFromHost();
        socket->deleteLater();
    }

    m_sockets.clear();
}

void
JeopardyServer::OnClientMessage()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(QObject::sender());
    if(!socket)
    {
        emit ServerMessage("Unable to cast to QObject::sender()", ERROR);
        return;
    }

    auto message = socket->readAll();
    const QString str = QString(message.constData());

    emit ServerMessage("Attempting to parse message: " + message, NONE);
    const auto pair = GameStateUtils::StateAction::GenerateFromString(str);

    if(pair.first)
    {
        emit ServerMessage("Message from " + QString::number(socket->socketDescriptor()) + ": " + str, RECEIVED);

        const GameStateUtils::StateAction& action = pair.second;

        if( action.state == GameState::SERVER_GAME_JEOPARDY)
        {
            m_gamePlayersReady[Jeopardy]++;

            if( m_gamePlayersReady[Jeopardy] == 2)
            {
                m_server->pauseAccepting();

                m_serverGameState = GameState::SERVER_START_MENU;

                GameStateUtils::StateResponse response;
                response.state = m_serverGameState;
                auto message = response.ToString().toLocal8Bit();

                // tell both players the server has connected to both
                // and is ready to start the game when the players are.
                for( auto socket : m_sockets)
                {
                    connect( socket, &QTcpSocket::readyRead, this, &JeopardyServer::OnClientMessage );
                    emit ServerMessage(tr("Sending message to both players: ") + message, SENT);
                    socket->write( message );
                }
            }
        }
        else if( action.state == GameState::SERVER_GAME_BATTLESHIP)
        {

        }
        else if( action.state == GameState::SERVER_START_MENU)
        {
            m_playersReadyToPlay++;

            emit ServerMessage(QString::number(socket->socketDescriptor()) + " is ready to play.", NONE);

            if(m_playersReadyToPlay == 2)
            {
                // This requires a little state setting trickery
                // so that the client can properly load the game now
                // instead of waiting for another message from a client

                GameStateUtils::StateAction action;
                action.state = GameState::MENU;

                auto response = m_game->DoStateAction(action);
                response.state = GameState::SERVER_GAME_START;
                SendResponseToClients(response);
            }
        }
        else
        {
            // Will this always be the case? Are there actions for which both players
            // need to properly enter state into the Game?

            if( m_serverGameState == action.state)
            {
                const auto response = m_game->DoStateAction(action);
                SendResponseToClients(response);
                emit ServerMessage( "Server state is " + GameStateString[m_serverGameState] + " after action on " + GameStateString[action.state] + ".", NONE);
            }
            else
            {
                emit ServerMessage("Ignoring this message because server state doesn't equal the client action state.", IGNORED);
                emit ServerMessage("Server state is " + GameStateString[m_serverGameState] + " - ignored action on state - " + GameStateString[action.state] + ".", NONE);
            }
        }
    }
    else
    {
        emit ServerMessage("Parse failed " + message, ERROR);
    }
}

void
JeopardyServer::SendResponseToClients(const GameStateUtils::StateResponse& response)
{
    if(response.state == GameState::INVALID)
        return;

    // The clients will send back an action on the BOARD
    // instead of SERVER_GAME_START/BOARD_START so we need to set
    // server state to BOARD since that we will be receiving next.
    switch( response.state )
    {
    case GameState::SERVER_GAME_START:
    case GameState::BOARD_START:
        m_serverGameState = GameState::BOARD;
        break;

    default:
        m_serverGameState = response.state;
    }

    const auto message = response.ToString().toLocal8Bit();
    for( auto socket : m_sockets)
    {
        socket->write( message );
    }

    // log the sent message
    emit ServerMessage(tr("Sending message to both players: ") + message, SENT);
}

void
JeopardyServer::CloseServer()
{
    m_serverGameState = GameState::SERVER_OFFLINE;

    if( m_server->isListening())
    {
        m_server->close();
    }

    for( auto socket : m_sockets)
    {
        socket->disconnectFromHost();
        socket->deleteLater();
    }

    m_sockets.clear();
}

JeopardyServer::~JeopardyServer()
{
    CloseServer();
}

