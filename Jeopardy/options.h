#ifndef OPTIONS_H
#define OPTIONS_H

typedef unsigned int UINT;

struct TimeIntervals
{
    // These are times in milliseconds
    UINT ClueQuestion;      // 10000
    UINT FinalStart;        // 3000
    UINT FinalCategory;     // 7000
    UINT FinalQuestion;     // 30000
    UINT FinalAnswer;       // 5000
    UINT AutoPlayAnimation; //400
    UINT AutoPlayFinal;     //1500
    UINT ClueTimeOut;       // 2500
    UINT ClueAnswer;        // 2000
    UINT GameOver;          // 5000

    // this will set the defaults
    TimeIntervals();
};

struct MusicOptions
{
    bool playFinalJeopardy; // true
    int volume;             // 50

    MusicOptions();
};

struct NextClueOptions
{
    // these are integer percent chances 0 to 100%
    int NewColumnChance;        // 10
    int NextRowSameColumnChance;// 90
    int NextRowNewColumnChance; // 70

    NextClueOptions();
};

struct OptionsData
{
    TimeIntervals       m_timeIntervals;
    MusicOptions        m_music;
    NextClueOptions     m_nextClueOptions;
};

#endif // OPTIONS_H
