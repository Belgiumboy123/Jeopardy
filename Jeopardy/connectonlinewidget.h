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

    std::unique_ptr<Ui::ConnectOnlineWidget> m_ui;
};

#endif // CONNECTONLINEWIDGET_H
