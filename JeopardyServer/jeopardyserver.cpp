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

    m_server =  new QTcpServer(this);
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
    else    // Sucess!
    {
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
        // TODO setup connections here
        // disconnected
        // readyReady

        m_sockets << socket;

        emit ServerMessage( QString::number(socket->socketDescriptor()) + tr(": made Connection.") );

        if( m_sockets.size() == 2)
        {
            m_server->pauseAccepting();

            // send message to sockets about having enough players.
            for( auto socket : m_sockets)
            {
                // TODO send out a proper State Response

                QString message("Start Game");
                socket->write( message.toLocal8Bit() );
            }
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

