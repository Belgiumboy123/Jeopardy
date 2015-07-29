#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_socket(new QTcpSocket(this))
{
    m_ui->setupUi(this);

    m_ui->portEdit->setValidator( new QIntValidator(1025, 65535, this));

    connect( m_ui->connectButton, &QPushButton::clicked, this, &MainWindow::OnConnectClicked);

    connect( m_socket, &QAbstractSocket::hostFound, this, &MainWindow::OnHostFound);
    connect( m_socket, static_cast<void (QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, &MainWindow::OnSocketError);
    connect( m_socket, &QAbstractSocket::connected, this, &MainWindow::OnSocketConnected);
    connect( m_socket, &QAbstractSocket::stateChanged, this, &MainWindow::OnStateChanged);
    connect( m_socket, &QAbstractSocket::disconnected, this, &MainWindow::OnDisconnected);
}

void
MainWindow::OnStateChanged(QAbstractSocket::SocketState socketState)
{
    QString text;

    switch(socketState)
    {
    case QAbstractSocket::UnconnectedState: text = "UnconnectedState"; break;
    case QAbstractSocket::HostLookupState: text = "HostLookupState"; break;
    case QAbstractSocket::ConnectingState: text = "ConnectingState"; break;
    case QAbstractSocket::ConnectedState: text = "Connected"; break;
    case QAbstractSocket::BoundState: text = "Bound"; break;
    case QAbstractSocket::ClosingState: text = "Closing"; break;
    case QAbstractSocket::ListeningState: text = "Listening"; break;
    }

    m_ui->resultLabel->setText(text);
}

void
MainWindow::OnConnectClicked()
{
    const QString& serverName = m_ui->serverNameEdit->text();
    bool isInt;
    const int portNumber = m_ui->portEdit->text().toInt(&isInt);

    if( serverName.isEmpty() || !isInt)
    {
        m_ui->resultLabel->setText("Oops, server name or port number is in incorrect format; please try again.");
    }
    else
    {
        m_socket->connectToHost(serverName, portNumber);
        m_ui->resultLabel->setText("Attempting to connect to server.");
    }

    m_ui->connectButton->hide();
}

void
MainWindow::OnSocketConnected()
{
    m_ui->resultLabel->setText("Connected to Server!");
}

void
MainWindow::OnDisconnected()
{
    m_ui->resultLabel->setText("Disconnected!");
}

void
MainWindow::OnHostFound()
{
    m_ui->resultLabel->setText("Host Found! Waiting to be connected.");
}

void
MainWindow::OnSocketError(QAbstractSocket::SocketError socketError)
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

    m_ui->resultLabel->setText( tr("Error: %1.").arg(errorString));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
