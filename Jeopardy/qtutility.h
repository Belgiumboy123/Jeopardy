#pragma once

#include <QFont>

#define CLUE_BLUE "#0A06B3"
#define BOARD_TEXT "#E29D44"

#define CLUE_FONT_SIZE 34

namespace QtUtil
{
    QFont GetBasicClueFont();

    QFont GetBasicBoardFont();

    QFont GetBasicTitleFont();
}
