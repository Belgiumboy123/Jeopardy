#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "jeopardyserver.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_server(new JeopardyServer)
{
    m_ui->setupUi(this);

    // Load the two needed fonts from resources.qrc
    QFontDatabase::addApplicationFont(":/korinna");
    QFontDatabase::addApplicationFont(":/swiss");

    QFont titleFont( "Swiss 911", 120, QFont::Normal );
    titleFont.setLetterSpacing( QFont::AbsoluteSpacing, 2 );

    QFont controlFont( "Korinna BT", 40, QFont::Normal );

    setAutoFillBackground(true);
    auto windowPal = palette();
    windowPal.setColor(backgroundRole(), QColor("#0A06B3"));
    setPalette(windowPal);

    auto titlePal = m_ui->title->palette();
    titlePal.setColor(m_ui->title->foregroundRole(), "#E29D44");
    m_ui->title->setPalette(titlePal);
    m_ui->title->setFont(titleFont);

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

    connect( m_server.get(), &JeopardyServer::ServerMessage, this, &MainWindow::OnServerMessage);

    showMaximized();
}

void
MainWindow::OnStartClicked()
{
    const int portNumber = m_ui->portEdit->text().toInt();
    auto result = m_server->StartServer(portNumber);

    if(!result.first)
    {
        m_ui->resultLabel->setText(result.second);
    }
    else
    {
        // server was started successfully!
        m_ui->resultLabel->setText("");
        m_ui->startWidget->hide();
        m_ui->portEdit->hide();
        m_ui->portLabel->hide();
        m_ui->serverStartLabel->setText( result.second );
        m_ui->textEdit->show();
        m_ui->serverStartLabel->show();
        m_ui->closeButton->show();
    }
}

void
MainWindow::OnServerMessage(const QString& message)
{
    qDebug() << message;
    m_ui->textEdit->appendPlainText(message);
}

void
MainWindow::OnCloseServer()
{
    m_server->CloseServer();

    m_ui->textEdit->clear();
    m_ui->closeButton->hide();
    m_ui->serverStartLabel->hide();
    m_ui->textEdit->hide();
    m_ui->startWidget->show();
    m_ui->portEdit->show();
    m_ui->portLabel->show();
}

MainWindow::~MainWindow() {}
