#pragma once

#include <QModelIndex>
#include <QWidget>

#include "options.h"

class QMediaPlayer;
class QStandardItemModel;
class QTimer;
class IStateHandler;
namespace GameStateUtils
{
    enum class GameState;
}

namespace Ui {
class GamePaneWidget;
}

class JeopardyGameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JeopardyGameWidget(QWidget *parent = nullptr);
    virtual ~JeopardyGameWidget();

    void StartGame(std::unique_ptr<IStateHandler> stateHandler);
    void SetOptions(const OptionsData& options);
    void SetAutoPlayEnabled(bool flag);

signals:
    void GameOver();

protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);

private:
    void OnStateChanged(GameStateUtils::GameState state, const QModelIndex& index, const QString& message);

    void handleBoardClick(const QModelIndex& index);
    void handleClueClick();

    std::unique_ptr<Ui::GamePaneWidget> m_ui;

    OptionsData& m_options;
    TimeIntervals& m_timeIntervals;

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
    void AutoPlayNextClue(const QModelIndex& index);

    QTimer* m_autoPlayTimer;
    void OnAutoPlayTimer();
    void StartAutoPlayTimer();

    std::unique_ptr<QMediaPlayer> m_mediaPlayer;
    void UpdateMediaPlayerFromOptions();

    struct PauseState
    {
        int clueTimeLeft;
        int timeOverLeft;
        int autoPlayLeft;
        int mediaPosition;
    };

    void launchPauseDialog();

    std::unique_ptr<IStateHandler> m_stateHandler;
};
