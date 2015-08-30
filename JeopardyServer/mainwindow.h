#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTcpServer;
class QTcpSocket;

class JeopardyServer;
enum class JeopardyServerMessageType;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void OnStartClicked();
    void OnServerMessage(const QString& message, const JeopardyServerMessageType& type);
    void OnCloseServer();

    std::unique_ptr<Ui::MainWindow> m_ui;

    std::unique_ptr<JeopardyServer> m_server;
};

#endif // MAINWINDOW_H
