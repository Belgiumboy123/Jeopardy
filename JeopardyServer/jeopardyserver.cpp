#include "jeopardyserver.h"

#include "jeopardygame.h"

#include <QNetworkInterface>
#include <QTcpSocket>
#include <QTcpServer>

using GameStateUtils::GameState;

JeopardyServer::JeopardyServer()
    : QObject()
{
    // TODO initialize with jeopardy server options
    m_game.reset(new JeopardyGame);

    m_serverGameState = GameState::SERVER_OFFLINE;

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

        emit ServerMessage("Server started.");
        connect( m_server, &QTcpServer::newConnection, this, &JeopardyServer::OnNewConnection);
    }

    return pair;
}

void
JeopardyServer::OnNewConnection()
{
    if( m_sockets.size() < 2)
    {
        QTcpSocket* socket = m_server->nextPendingConnection();
        if(!socket)
            return;

        // TODO disconnected signal
        // todo should this connection be moved down to when we are in start menu?
        connect( socket, &QTcpSocket::readyRead, this, &JeopardyServer::OnClientMessage );

        m_sockets << socket;

        emit ServerMessage( QString::number(socket->socketDescriptor()) + tr(": made Connection.") );

        if( m_sockets.size() == 2)
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
                emit ServerMessage(tr("Sending message to both players: ") + message);
                socket->write( message );
            }
        }
    }
}

void
JeopardyServer::OnClientMessage()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(QObject::sender());
    if(!socket)
        return;

    auto message = socket->readAll();
    const QString str = QString(message.constData());
    const auto pair = GameStateUtils::StateAction::GenerateFromString(str);

    if(pair.first)
    {
        emit ServerMessage(QString::number(socket->socketDescriptor()) + ": " + str);

        const GameStateUtils::StateAction& action = pair.second;

        if( action.state == GameState::SERVER_START_MENU)
        {
            // TODO if both clients have hit this action
            // send out start game message.

            // send jeopardy game instance with a action with state GameState::MENU
            // set jeopardy server game state to match return value of jeopardy game response
        }
    }
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

JeopardyServer::~JeopardyServer() {}

