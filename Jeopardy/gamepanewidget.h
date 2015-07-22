#ifndef GAMEPANEWIDGET_H
#define GAMEPANEWIDGET_H

#include <QModelIndex>
#include <QWidget>

#include "options.h"

class JeopardyGame;
class QMediaPlayer;
class QStandardItemModel;
class QTimer;

namespace Ui {
class GamePaneWidget;
}

class GamePaneWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GamePaneWidget(QWidget *parent = 0);
    virtual ~GamePaneWidget();

    void StartGame();
    void SetOptions(const OptionsData& options);
    void SetAutoPlayEnabled(bool flag);

    void PauseGame();
    void ContinueGame();

signals:
    void GameOver();

protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);

private:
    void handleBoardClick(const QModelIndex& index);
    void handleClueClick();

    void SetNewClueQuestion(const QModelIndex& index, const QString& question);

    std::unique_ptr<Ui::GamePaneWidget> m_ui;

    OptionsData m_options;
    TimeIntervals m_timeIntervals;

    std::unique_ptr<JeopardyGame> m_game;

    enum GameState
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
        GAME_OVER,
        PAUSED
    };
    GameState m_mode;

    struct AutoPlayAnimationState
    {
        QModelIndex newIndex;
        int currColumn;
        int currRow;
        int columnDirection;
        int rowDirection;
    };
    AutoPlayAnimationState m_autoPlayState;

    QModelIndex m_clickedIndex;

    QTimer* m_clueTimer;
    void OnClueTimerOut();

    void StartTimeOverTimer(const unsigned int milliSeconds);

    QTimer* m_timeOverTimer;
    void OnTimeOverTimerOut();

    void StartClueTimer( const unsigned int milliSeconds);

    bool m_isAutoPlayEnabled;
    bool IsAutoPlayEnabled() const;
    void AutoPlayNextClue();

    QTimer* m_autoPlayTimer;
    void OnAutoPlayTimer();
    void StartAutoPlayTimer();

    std::unique_ptr<QMediaPlayer> m_mediaPlayer;
    void UpdateMediaPlayerFromOptions();

    struct PauseState
    {
        GameState mode;
        int clueTimeLeft;
        int timeOverLeft;
        int autoPlayLeft;
        int mediaPosition;
    };
    PauseState m_pauseState;
};

#endif // GAMEPANEWIDGET_H