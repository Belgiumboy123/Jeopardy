#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QNetworkInterface>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_server( new QTcpServer(this))
{
    m_ui->setupUi(this);




    connect( m_ui->startButton, &QPushButton::clicked, this, &MainWindow::OnStartClicked);

    m_ui->portEdit->setValidator( new QIntValidator(1025, 65535, this));

    m_ui->serverStartLabel->hide();
}

void
MainWindow::OnStartClicked()
{
    QHostAddress address(QHostAddress::Any);
    const int portNumber = m_ui->portEdit->text().toInt();

    if( !m_server->listen(address, portNumber))
    {
        m_ui->resultLabel->setText( tr("Unable to start the server: %1.").arg(m_server->errorString()));
    }
    else
    {   // server was started successfully!
        m_ui->startButton->hide();

        m_ui->resultLabel->setText("Server started succesfully.");

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

        m_ui->serverStartLabel->setText( tr("Server started on IP: %1 on host %2.").arg(ipAddress).arg(portNumber) );
        m_ui->serverStartLabel->show();
        m_ui->portEdit->hide();
        m_ui->portLabel->hide();
        connect( m_server, &QTcpServer::newConnection, this, &MainWindow::OnNewConnection);
    }
}

void
MainWindow::OnNewConnection()
{
    QTcpSocket* socket = m_server->nextPendingConnection();

    auto socketDescriptor = socket->socketDescriptor();

    auto result = m_ui->resultLabel->text();
    result += tr("\n") + tr("Connection made ") + QString::number(socketDescriptor);

    m_ui->resultLabel->setText(result);
}

MainWindow::~MainWindow() {}
