#ifndef OPTIONS_H
#define OPTIONS_H

struct TimeIntervals
{
    // These are times in milliseconds
    int ClueQuestion;      // 10000
    int FinalStart;        // 3000
    int FinalCategory;     // 7000
    int FinalQuestion;     // 30000
    int FinalAnswer;       // 5000
    int AutoPlayAnimation; //400
    int AutoPlayFinal;     //1500
    int ClueTimeOut;       // 2500
    int ClueAnswer;        // 2000
    int GameOver;          // 5000

    // this will set the defaults
    TimeIntervals();
};

struct SoundOptions
{
    bool playFinalJeopardy; // true
    int volume;             // 50

    SoundOptions();
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
    SoundOptions        m_music;
    NextClueOptions     m_nextClueOptions;
};

#endif // OPTIONS_H
