#include "options.h"

TimeIntervals::TimeIntervals()
    : ClueQuestion(10000)
    , FinalStart(3000)
    , FinalCategory(7000)
    , FinalQuestion(30000)
    , FinalAnswer(5000)
    , AutoPlayAnimation(400)
    , AutoPlayFinal(1500)
    , ClueTimeOut(2500)
    , ClueAnswer(2000)
    , GameOver(5000)
{
}

SoundOptions::SoundOptions()
    : playFinalJeopardy(true)
    , volume(50)
{
}

NextClueOptions::NextClueOptions()
    : NewColumnChance(10)
    , NextRowSameColumnChance(90)
    , NextRowNewColumnChance(70)
{
}
