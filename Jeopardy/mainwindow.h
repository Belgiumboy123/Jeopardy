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
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);
    virtual void closeEvent(QCloseEvent *event);

private:
    void handleStartGameClick();
    void OnGameOver();

    std::unique_ptr<Ui::MainWindow> m_ui;

    OptionsData m_options;

    enum GameState
    {
        MENU = 0,
        GAME,
        GAME_OVER,
        PAUSED
    };
    GameState m_mode;

    void launchOptionsDialog();
    void launchPauseDialog();
};

#endif // MAINWINDOW_H
