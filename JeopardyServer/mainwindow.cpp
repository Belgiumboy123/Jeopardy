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

    m_ui->servertStartLabel->hide();
}

void
MainWindow::OnStartClicked()
{
    QHostAddress address(QHostAddress::Any);
    const int portNumber = m_ui->portEdit->text().toInt();

    m_ui->portEdit->hide();
    m_ui->portLabel->hide();
    m_ui->servertStartLabel->show();

    if( !m_server->listen(address, portNumber))
    {
        m_ui->servertStartLabel->setText( tr("Unable to start the server: %1.").arg(m_server->errorString()));

        // TODO show a try again button, or show this label else where without hiding port widget
    }
    else
    {   // server was started successfully!
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

        m_ui->servertStartLabel->setText( tr("Server started on IP: %1 on host %2.").arg(ipAddress).arg(portNumber) );
        connect( m_server, &QTcpServer::newConnection, this, &MainWindow::OnNewConnection);
    }
}

void
MainWindow::OnNewConnection()
{
    QTcpSocket* socket = m_server->nextPendingConnection();

    auto socketDescriptor = socket->socketDescriptor();

    qDebug() << "Connection made " << socketDescriptor;
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
