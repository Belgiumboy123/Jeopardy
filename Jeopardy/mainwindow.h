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

    void OnOnlineButtonClicked();
    void OnOfflineButtonClicked();

    std::unique_ptr<Ui::MainWindow> m_ui;

    enum GameState
    {
        PICK_OFFLINE_ONLINE = 0,
        OFFLINE_MENU,
        ONLINE_MENU,
        GAME,
        GAME_OVER,
        PAUSED
    };
    GameState m_gameState;

    void ShowGameState(GameState gameState);

    void launchOptionsDialog();
};

#endif // MAINWINDOW_H
