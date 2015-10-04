#include "jeopardyservernoui.h"

#include "jeopardyserver.h"

#include <QDateTime>
#include <QFile>
#include <QTextStream>

JeopardyServerNoUi::JeopardyServerNoUi()
    : QObject()
    , m_server(new JeopardyServer)
{
    const auto dtString = QDateTime::currentDateTime().toString("MMM.dd.yyyy-HH.mm.ss.zzz");
    const QString filename = dtString + ".txt";
    m_logFile.reset(new QFile(filename));
    m_logFile->open(QIODevice::ReadWrite);
}

bool
JeopardyServerNoUi::Start(const QString& port)
{
    bool ok;
    int portNumber = port.toInt(&ok);

    if( ok )
    {
        return Start(portNumber);
    }

    return false;
}

bool
JeopardyServerNoUi::Start(const int portNumber)
{
    connect( m_server.get(), &JeopardyServer::ServerMessage, this, &JeopardyServerNoUi::OnServerMessage);

    auto result = m_server->StartServer(portNumber);
    return result.first;
}

void
JeopardyServerNoUi::OnServerMessage(const QString& message, const JeopardyServerMessageType& type)
{
    Q_UNUSED(type);

    const auto dtString = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    const QString timeMessage = dtString + QString("> ") + message + "\n";

    QTextStream textStream(m_logFile.get());
    textStream << timeMessage;
}

JeopardyServerNoUi::~JeopardyServerNoUi() {}

