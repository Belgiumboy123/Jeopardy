#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QModelIndex>

namespace Ui {
class MainWindow;
}

class JeopardyGame;
class QModelIndex;
class QTimer;
class QModelIndex;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);

private:

    void handleStartGameClick();
    void handleBoardClick(const QModelIndex& index);
    void handleClueClick();

    void SetNewClueQuestion(const QModelIndex& index, const QString& question);

    Ui::MainWindow *m_ui;

    std::unique_ptr<JeopardyGame> m_game;

    enum GameMode
    {
        MENU = 0,
        BOARD,
        CLUE_QUESTION,
        CLUE_ANSWER,
        CLUE_TIMEOUT,
        FINAL_START,
        FINAL_CATEGORY,
        FINAL_CLUE,
        FINAL_ANSWER,
        GAME_OVER
    };

    GameMode m_mode;

    QModelIndex m_clickedIndex;

    QTimer* m_clueTimer;
    void OnClueTimerOut();

    QTimer* m_timeOverTimer;
    void OnTimeOverTimerOut();

    void StartClueTimer( const unsigned int milliSeconds);

    bool IsAutoPlayEnabled() const;
};

#endif // MAINWINDOW_H
