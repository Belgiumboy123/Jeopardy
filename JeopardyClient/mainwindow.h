#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

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
    void OnConnectClicked();
    void OnHostFound();
    void OnSocketError(QAbstractSocket::SocketError socketError);
    void OnSocketConnected();
    void OnStateChanged(QAbstractSocket::SocketState socketState);
    void OnDisconnected();

    Ui::MainWindow *m_ui;

    QTcpSocket* m_socket;
};

#endif // MAINWINDOW_H
