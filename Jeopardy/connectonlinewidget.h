#ifndef CONNECTONLINEWIDGET_H
#define CONNECTONLINEWIDGET_H

#include <QWidget>

class StateHandlerOnline;
namespace Ui {
class ConnectOnlineWidget;
}

class ConnectOnlineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectOnlineWidget(QWidget *parent = 0);
    ~ConnectOnlineWidget();

    void BeginConnection(std::unique_ptr<StateHandlerOnline> stateHandler);

signals:
    void StartGame();
    void BackToMenu();

private:
    void OnConnectButton();
    void OnBackButton();
    void OnStartGameButton();

    void OnConnectionMade();
    void OnConnectionLost(const QString& message);
    void OnConnectionMessage(const QString& message);

    std::unique_ptr<Ui::ConnectOnlineWidget> m_ui;

    enum State
    {
        ENTRY = 0,
        CONNECTED,
        START
    } m_state;

    void ShowState( State state);

    std::unique_ptr<StateHandlerOnline> m_stateHandler;
};

#endif // CONNECTONLINEWIDGET_H
