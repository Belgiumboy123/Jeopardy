#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>

#include "options.h"

namespace Ui {
class MainWindow;
}

class QMediaPlayer;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent* event);

private:
    void OnOfflineGameStart();
    void OnGameOver();
    void OnBack();
    void OnOnlineGameStart();

    void OnJeoOnlineButtonClicked();
    void OnJeoOfflineButtonClicked();

    void OnJeopardyStart();
    void OnBattleShipStart();

    std::unique_ptr<Ui::MainWindow> m_ui;

    enum GameState
    {
        PICK_GAME = 0,
        PICK_JEO_OFFLINE_ONLINE,
        OFFLINE_MENU,
        ONLINE_JEO_MENU,
        ONLINE_BS_MENU,
        JEO_GAME,
        BS_GAME,
        GAME_OVER,
        PAUSED
    };
    GameState m_gameState;

    void ShowGameState(GameState gameState);

    void launchOptionsDialog();
};

#endif // MAINWINDOW_H
