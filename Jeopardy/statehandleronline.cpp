#include "statehandleronline.h"

#include "qtutility.h"

#include <QStandardItemModel>

using GameStateUtils::TOTAL_COLS;
using GameStateUtils::TOTAL_ROWS;
using GameStateUtils::GameState;

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

bool
StateHandlerOnline::AllowUserInteraction() const
{
    return false;
}

void
StateHandlerOnline::LoadModelFromCluesString(QString clues)
{
    if( clues.isEmpty())
    {
        return;
    }

    m_model->clear();

    bool parseSuccess = true;

    for( int column = 0; column<TOTAL_COLS; column++)
    {
        // add header item for category
        auto result = GameStateUtils::GetCategoryHeader(clues);
        if(!result.first) parseSuccess = false;

        QStandardItem* horizontalItem = new QStandardItem(result.second);
        m_model->setHorizontalHeaderItem(column, horizontalItem);

        for( int row = 0; row<TOTAL_ROWS; row++)
        {
            auto result = GameStateUtils::GetClueText(clues);
            if(!result.first) parseSuccess = false;

            QStandardItem* item = new QStandardItem;
            item->setText(result.second);
            item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            item->setForeground(QBrush(BOARD_TEXT));
            item->setBackground(QBrush(CLUE_BLUE));

            m_model->setItem(row, column, item);
        }
    }

    if( !parseSuccess)
    {
        m_model->clear();
    }
}

void
StateHandlerOnline::DoActionOnState(GameStateUtils::GameState currentState, const QModelIndex& index)
{
    if( currentState == GameState::MENU)
    {
        // Server loaded the board on state SERVER_START_GAME
        // so just return board start here.
        // TODO how to grab the index the server actually returned?

        const auto& responseIndex = GetModel()->index(0, 0);
        emit StateChanged(GameState::BOARD_START, responseIndex, "");
        return;
    }

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
        const GameStateUtils::StateResponse& response = pair.second;
        const auto& responseIndex = GetModel()->index(response.row, response.column);

        // $Mark use this as chat in when the game has started
        emit ConnectionMessage(response.message);

        switch(response.state)
        {
        case GameState::SERVER_START_MENU:
            emit BothPlayersConnected();
            return;

        case GameState::SERVER_GAME_START:
            LoadModelFromCluesString(response.serverClues);
            emit StartGame();
            return;

        case GameState::BOARD_START:
            LoadModelFromCluesString(response.serverClues);
            break;

        case GameState::CLUE_QUESTION:
            // Only emit state changed if clue isn't empty.
            // The clue might be empty because the either the clue has already been asked
            // Or the clue never existed to begin with
            if(response.message.isEmpty())
            {
                return;
            }

            GetModel()->itemFromIndex(responseIndex)->setText("");
            break;

        case GameState::OPPONENT_DISCONNECTED:
            emit ConnectionLost("Opponent has disconnected!");
            break;

        default:
            break;
        }

        emit StateChanged(response.state, responseIndex, response.message);
    }
    else
    {
        qDebug() << "Unable to parse message-------------------------------";
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

