#ifndef JEOPARDYDATABASE_H
#define JEOPARDYDATABASE_H


#include <map>
#include <vector>

#include <QString>

namespace DatabaseUtils
{
    int GetANonPlayedGame();

    typedef std::pair<QString,QString> ClueAnswerPair;
    typedef std::pair<int,ClueAnswerPair> ValuePair;
    typedef std::map<int,ClueAnswerPair> CategoryQuestions;
    typedef std::map<QString, CategoryQuestions> RoundQuestions;

    typedef std::pair<QString,ValuePair> CategoryQuestion;
    typedef std::vector<CategoryQuestion> CategoryQuestionsVector;

    struct StaticGameInfo
    {
        QString finalCategory;
        QString finalClue;
        QString finalAnswer;

        RoundQuestions singleRoundQuestions;
        RoundQuestions doubleRoundQuestions;

        int totalSingleClues;
        int totalDoubleClues;

        StaticGameInfo();
    };

    void GetGameInfo(const int gameID, StaticGameInfo& gameInfo);
}

#endif // JEOPARDYDATABASE_H
