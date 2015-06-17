#include "options.h"

#include <QDataStream>
#include <QSettings>

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

Q_DECLARE_METATYPE(OptionsData);

QDataStream& operator<<(QDataStream& out, const OptionsData& obj)
{
    out << obj.m_timeIntervals.AutoPlayAnimation << obj.m_timeIntervals.AutoPlayFinal;
    out << obj.m_timeIntervals.ClueAnswer << obj.m_timeIntervals.ClueQuestion;
    out << obj.m_timeIntervals.ClueTimeOut << obj.m_timeIntervals.FinalAnswer;
    out << obj.m_timeIntervals.FinalCategory << obj.m_timeIntervals.FinalQuestion;
    out << obj.m_timeIntervals.FinalStart << obj.m_timeIntervals.GameOver;
    out << obj.m_music.volume << obj.m_music.playFinalJeopardy;
    out << obj.m_nextClueOptions.NewColumnChance << obj.m_nextClueOptions.NextRowNewColumnChance;
    out << obj.m_nextClueOptions.NextRowSameColumnChance;
    return out;
}

QDataStream& operator>>(QDataStream& in, OptionsData& obj)
{

    in >> obj.m_timeIntervals.AutoPlayAnimation >> obj.m_timeIntervals.AutoPlayFinal;
    in >> obj.m_timeIntervals.ClueAnswer >> obj.m_timeIntervals.ClueQuestion;
    in >> obj.m_timeIntervals.ClueTimeOut >> obj.m_timeIntervals.FinalAnswer;
    in >> obj.m_timeIntervals.FinalCategory >> obj.m_timeIntervals.FinalQuestion;
    in >> obj.m_timeIntervals.FinalStart >> obj.m_timeIntervals.GameOver;
    in >> obj.m_music.volume >> obj.m_music.playFinalJeopardy;
    in >> obj.m_nextClueOptions.NewColumnChance >> obj.m_nextClueOptions.NextRowNewColumnChance;
    in >> obj.m_nextClueOptions.NextRowSameColumnChance;
    return in;
}

/*static*/ OptionsData
OptionsData::FromSettings()
{
    qRegisterMetaTypeStreamOperators<OptionsData>("OptionsData");
    QSettings settings("STAR", "jeopardy");
    if( settings.contains("options"))
    {
        QVariant value = settings.value("options");
        return value.value<OptionsData>();
    }
    else
    {
        return OptionsData();
    }
}

void
OptionsData::Save()
{
    QSettings settings("STAR", "jeopardy");
    settings.setValue("options", QVariant::fromValue(*this));
}
