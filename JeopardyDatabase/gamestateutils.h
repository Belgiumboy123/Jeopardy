#pragma once

#include <QString>

namespace GameStateUtils
{
    const QString DOLLAR = "$";

    const int TOTAL_ROWS = 5;
    const int TOTAL_COLS = 6;

    enum class GameState
    {
        MENU = 0,
        BOARD_START,
        BOARD,
        CLUE_QUESTION,
        CLUE_ANSWER,
        CLUE_TIMEOUT,
        FINAL_START,
        FINAL_CATEGORY,
        FINAL_CLUE,
        FINAL_ANSWER,
        GAME_OVER,
        PAUSED
    };
}

