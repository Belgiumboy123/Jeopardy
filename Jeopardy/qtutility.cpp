#include "qtutility.h"

#define CLUE_FONT "Korinna BT"
#define TITLE_FONT "Swiss 911"
#define BOARD_FONT "Triumvirate CG Inserat"

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

QFont QtUtil::GetBasicTitleFont()
{
    QFont titleFont( TITLE_FONT, 34, QFont::Normal );
    titleFont.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    return titleFont;
}
