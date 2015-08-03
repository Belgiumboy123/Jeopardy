#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTcpServer;
class QTcpSocket;

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
    void OnNewConnection();
    void OnCloseServer();

    std::unique_ptr<Ui::MainWindow> m_ui;

    QTcpServer* m_server;

    QList<QTcpSocket*> m_sockets;
};

#endif // MAINWINDOW_H
