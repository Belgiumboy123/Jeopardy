#pragma once

#include <QObject>
#include <memory>

class JeopardyServer;
enum class JeopardyServerMessageType;
class QFile;
class QString;


class JeopardyServerNoUi : public QObject
{
    Q_OBJECT

public:
    JeopardyServerNoUi();
    ~JeopardyServerNoUi();

    bool Start(const QString& port);

private:
    void OnServerMessage(const QString& message, const JeopardyServerMessageType& type);
    bool Start(const int portNumber);

    std::unique_ptr<JeopardyServer> m_server;
    std::unique_ptr<QFile> m_logFile;
};

