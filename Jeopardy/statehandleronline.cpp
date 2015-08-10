#include "statehandleronline.h"

StateHandlerOnline::StateHandlerOnline()
    : IStateHandler()
    , m_socket(new QTcpSocket(this))
{
    connect( m_socket, &QAbstractSocket::hostFound, this, &StateHandlerOnline::OnHostFound);
    connect( m_socket, static_cast<void (QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, &StateHandlerOnline::OnSocketError);
    connect( m_socket, &QAbstractSocket::connected, this, &StateHandlerOnline::OnSocketConnected);
    connect( m_socket, &QAbstractSocket::stateChanged, this, &StateHandlerOnline::OnStateChanged);
    connect( m_socket, &QAbstractSocket::disconnected, this, &StateHandlerOnline::OnDisconnected);
}

StateHandlerOnline::~StateHandlerOnline() {}

void
StateHandlerOnline::DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index)
{
    GameStateUtils::StateAction action;
    action.column = index.column();
    action.row = index.row();
    action.state = currentState;

    m_socket->write( action.ToString().toLocal8Bit() );
}

void
StateHandlerOnline::SetNextClueOptions(const NextClueOptions& nextClueOptions)
{
    Q_UNUSED(nextClueOptions);
}

void
StateHandlerOnline::ConnectToHost(const QString& hostname, const int port)
{
    m_socket->connectToHost(hostname, port);
}

void
StateHandlerOnline::OnSocketConnected()
{
    connect( m_socket, &QTcpSocket::readyRead, this, &StateHandlerOnline::OnServerMessage );

    emit ConnectionMade();
}

void
StateHandlerOnline::OnServerMessage()
{
    auto message = m_socket->readAll();
    QString str = QString(message.constData());
    auto pair = GameStateUtils::StateResponse::GenerateFromString(str);

    if(pair.first)
    {
         emit ConnectionMessage(str);
    }
}

void
StateHandlerOnline::OnDisconnected()
{
    emit ConnectionLost("Connection was lost!");
}

void
StateHandlerOnline::OnHostFound()
{
    emit ConnectionMessage("Host Found! Waiting to be connected...");
}

void
StateHandlerOnline::OnSocketError(QAbstractSocket::SocketError socketError)
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
StateHandlerOnline::OnStateChanged(QAbstractSocket::SocketState socketState)
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

