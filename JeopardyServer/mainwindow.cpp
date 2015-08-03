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

    setAutoFillBackground(true);
    auto windowPal = palette();
    windowPal.setColor(backgroundRole(), QColor("#0A06B3"));
    setPalette(windowPal);

    auto titlePal = m_ui->title->palette();
    titlePal.setColor(m_ui->title->foregroundRole(), "#E29D44");
    m_ui->title->setPalette(titlePal);
    auto titleFont = m_ui->title->font();
    titleFont.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    titleFont.setPointSize(120);
    m_ui->title->setFont(titleFont);

    QFont controlFont(font());
    controlFont.setPointSize(40);

    m_ui->portLabel->setFont(controlFont);
    auto portPal = m_ui->portLabel->palette();
    portPal.setColor( m_ui->portLabel->foregroundRole(), Qt::white);
    m_ui->portLabel->setPalette(portPal);

    m_ui->startButton->setFont(controlFont);
    connect( m_ui->startButton, &QPushButton::clicked, this, &MainWindow::OnStartClicked);

    m_ui->portEdit->setFont(controlFont);
    m_ui->portEdit->setValidator( new QIntValidator(1025, 65535, this));
    connect( m_ui->portEdit, &QLineEdit::returnPressed, this, &MainWindow::OnStartClicked);

    m_ui->resultLabel->setFont(controlFont);
    auto resultPal = m_ui->resultLabel->palette();
    resultPal.setColor( m_ui->resultLabel->foregroundRole(), Qt::red);
    m_ui->resultLabel->setPalette(resultPal);

    m_ui->serverStartLabel->setFont(controlFont);
    auto serverStartPal = m_ui->serverStartLabel->palette();
    serverStartPal.setColor( m_ui->serverStartLabel->foregroundRole(), Qt::white);
    m_ui->serverStartLabel->setPalette(serverStartPal);

    m_ui->textEdit->setReadOnly(true);
    m_ui->textEdit->setTabChangesFocus(true);
    auto editFont(m_ui->textEdit->font());
    editFont.setPointSize(28);
    m_ui->textEdit->setFont(editFont);

    m_ui->closeButton->setFont(controlFont);
    connect( m_ui->closeButton, &QPushButton::clicked, this, &MainWindow::OnCloseServer);

    m_ui->closeButton->hide();
    m_ui->serverStartLabel->hide();
    m_ui->textEdit->hide();

    showMaximized();
}

void
MainWindow::OnStartClicked()
{
    QHostAddress address(QHostAddress::Any);
    const int portNumber = m_ui->portEdit->text().toInt();

    if(!m_server->listen(address, portNumber))
    {
        m_ui->resultLabel->setText( tr("Unable to start the server: %1.").arg(m_server->errorString()));
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

        m_ui->resultLabel->setText("");
        m_ui->startWidget->hide();
        m_ui->portEdit->hide();
        m_ui->portLabel->hide();
        m_ui->serverStartLabel->setText( tr("Server started on IP: %1 on host %2.").arg(ipAddress).arg(portNumber) );
        m_ui->textEdit->appendPlainText("Server started.");
        m_ui->textEdit->show();
        m_ui->serverStartLabel->show();
        m_ui->closeButton->show();
        connect( m_server, &QTcpServer::newConnection, this, &MainWindow::OnNewConnection);
    }
}

void
MainWindow::OnNewConnection()
{
    QTcpSocket* socket = m_server->nextPendingConnection();

    auto socketDescriptor = socket->socketDescriptor();

    auto result = QString::number(socketDescriptor) + tr(": made Connection.");
    m_ui->textEdit->appendPlainText(result);
}

void
MainWindow::OnCloseServer()
{
    if( m_server->isListening())
    {
        m_server->close();
    }

    m_ui->closeButton->hide();
    m_ui->serverStartLabel->hide();
    m_ui->textEdit->hide();
    m_ui->startWidget->show();
    m_ui->portEdit->show();
    m_ui->portLabel->show();
}

MainWindow::~MainWindow() {}
