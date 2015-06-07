#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QModelIndex>

#include "autoplayoptionsdialog.h"

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
        CLUE_ANIMATION,
        FINAL_START,
        FINAL_CATEGORY,
        FINAL_CLUE,
        FINAL_ANSWER,
        GAME_OVER
    };
    GameMode m_mode;

    struct AutoPlayAnimationState
    {
        QModelIndex newIndex;
        int currColumn;
        int currRow;
        int columnDirection;
        int rowDirection;
    };
    AutoPlayAnimationState m_autoPlayState;

    TimeIntervals m_timeIntervals;

    QModelIndex m_clickedIndex;

    QTimer* m_clueTimer;
    void OnClueTimerOut();

    void StartTimeOverTimer(const unsigned int milliSeconds);

    QTimer* m_timeOverTimer;
    void OnTimeOverTimerOut();

    void StartClueTimer( const unsigned int milliSeconds);

    bool IsAutoPlayEnabled() const;
    void AutoPlayNextClue();

    QTimer* m_autoPlayTimer;
    void OnAutoPlayTimer();
    void StartAutoPlayTimer();

    void launchAutoPlayOptionsDialog();
};

#endif // MAINWINDOW_H
