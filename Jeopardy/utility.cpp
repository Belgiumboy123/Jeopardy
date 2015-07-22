#include "utility.h"


QFont Util::GetBasicClueFont()
{
    return QFont( CLUE_FONT, CLUE_FONT_SIZE, QFont::Normal );
}

QFont Util::GetBasicBoardFont()
{
    QFont boardFont( BOARD_FONT, 34, QFont::Normal );
    boardFont.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    return boardFont;
}
