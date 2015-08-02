#ifndef CONNECTONLINEWIDGET_H
#define CONNECTONLINEWIDGET_H

#include <QWidget>

namespace Ui {
class ConnectOnlineWidget;
}

class ConnectOnlineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectOnlineWidget(QWidget *parent = 0);
    ~ConnectOnlineWidget();

signals:
    void StartGame();
    void BackToMenu();

private:
    void OnConnectButton();
    void OnBackButton();
    void OnStartGameButton();

    std::unique_ptr<Ui::ConnectOnlineWidget> m_ui;

    enum State
    {
        ENTRY = 0,
        CONNECTED,
        START
    } m_state;

    void ShowState( State state);
};

#endif // CONNECTONLINEWIDGET_H
