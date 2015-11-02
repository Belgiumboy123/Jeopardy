#include "istatehandleronline.h"

using GameStateUtils::GameState;

IStateHandlerOnline::IStateHandlerOnline()
    : IStateHandler()
    , m_socket(new QTcpSocket(this))
{
    connect( m_socket, &QAbstractSocket::hostFound, this, &IStateHandlerOnline::OnHostFound);
    connect( m_socket, static_cast<void (QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, &IStateHandlerOnline::OnSocketError);
    connect( m_socket, &QAbstractSocket::connected, this, &IStateHandlerOnline::OnSocketConnected);
    connect( m_socket, &QAbstractSocket::stateChanged, this, &IStateHandlerOnline::OnStateChanged);
    connect( m_socket, &QAbstractSocket::disconnected, this, &IStateHandlerOnline::OnDisconnected);
}

IStateHandlerOnline::~IStateHandlerOnline()
{
}

void
IStateHandlerOnline::ConnectToHost(const QString& hostname, const int port)
{
    m_socket->connectToHost(hostname, port);
}

void
IStateHandlerOnline::OnDisconnected()
{
    emit ConnectionLost("Connection was lost!");
}

void
IStateHandlerOnline::OnHostFound()
{
    emit ConnectionMessage("Host Found! Waiting to be connected...");
}

void
IStateHandlerOnline::OnSocketConnected()
{
    connect( m_socket, &QTcpSocket::readyRead, this, &IStateHandlerOnline::OnServerMessage );

    emit ConnectionMade();
}

void
IStateHandlerOnline::DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index /*= QModelIndex()*/)
{
    GameStateUtils::StateAction action;
    action.column = index.column();
    action.row = index.row();
    action.state = currentState;

    m_socket->write( action.ToString().toLocal8Bit() );
}

void
IStateHandlerOnline::OnServerMessage()
{
    auto message = m_socket->readAll();
    QString str = QString(message.constData());
    auto pair = GameStateUtils::StateResponse::GenerateFromString(str);

    if(pair.first)
    {
        const GameStateUtils::StateResponse& response = pair.second;

        // $Mark use this as chat in when the game has started
        emit ConnectionMessage(response.message);

        switch(response.state)
        {
        case GameState::SERVER_START_MENU:
            emit BothPlayersConnected();
            return;

        case GameState::OPPONENT_DISCONNECTED:
            emit ConnectionLost("Opponent has disconnected!");
            return;

        default:
            break;
        }

        // Let derived state handlers do their thing
        HandleServerResponse();
    }
    else
    {
        qDebug() << "Unable to parse message-------------------------------";
    }
}

void
IStateHandlerOnline::OnSocketError(QAbstractSocket::SocketError socketError)
{
    QString errorString;

    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        errorString = "Remote Host Closed";
        break;

    case QAbstractSocket::HostNotFoundError:
        errorString = "The host was not found. Please check the host name and port settings.";
        break;

    case QAbstractSocket::ConnectionRefusedError:
        errorString = "The connection was refused by the peer. Make sure the fortune server is running, and check that the host name and port settings are correct.";
        break;

    default:
        errorString = m_socket->errorString();
    }

    emit ConnectionLost( tr("Error: %1.").arg(errorString) );
}

void
IStateHandlerOnline::OnStateChanged(QAbstractSocket::SocketState socketState)
{
    QString text;

    switch(socketState)
    {
    case QAbstractSocket::UnconnectedState: text = "Unconnected"; break;
    case QAbstractSocket::HostLookupState: text = "Looking up host"; break;
    case QAbstractSocket::ConnectingState: text = "Connecting..."; break;
    case QAbstractSocket::ConnectedState: text = "Connected"; break;
    case QAbstractSocket::BoundState: text = "Bound"; break;
    case QAbstractSocket::ClosingState: text = "Closing"; break;
    case QAbstractSocket::ListeningState: text = "Listening"; break;
    default: return;
    }

    emit ConnectionMessage(text);
}

bool
IStateHandlerOnline::AllowUserInteraction() const
{
    return false;
}

void
IStateHandlerOnline::SetNextClueOptions(const NextClueOptions& nextClueOptions)
{
    Q_UNUSED(nextClueOptions);
}

