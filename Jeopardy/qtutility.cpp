#include "qtutility.h"


QFont QtUtil::GetBasicClueFont()
{
    return QFont( CLUE_FONT, CLUE_FONT_SIZE, QFont::Normal );
}

QFont QtUtil::GetBasicBoardFont()
{
    QFont boardFont( BOARD_FONT, 34, QFont::Normal );
    boardFont.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    return boardFont;
}
