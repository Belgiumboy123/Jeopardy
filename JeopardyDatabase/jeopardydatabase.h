#ifndef JEOPARDYDATABASE_H
#define JEOPARDYDATABASE_H

#include "gamestateutils.h"

#include <map>
#include <vector>

#include <QString>

namespace DatabaseUtils
{
    QString GetFilePathAppResourcesFile(const QString& filename);

    int GetANonPlayedGame();

    void UseUnitTestDatabasePath();

    struct JeopardyGameInfo
    {
        QString finalCategory;
        QString finalClue;
        QString finalAnswer;

        GameStateUtils::Clues singleRoundClues{false};
        GameStateUtils::Clues doubleRoundClues{true};

        // hide this behind some functions, should be private
        // and other getter functions will all have to be sure to go through
        // those functions instead of directly accessing the above clues
        GameStateUtils::Clues& currentClues{singleRoundClues};

        int totalSingleClues;
        int totalDoubleClues;

        JeopardyGameInfo();
        void clear();
    };

    void GetJeopardyGameInfo(const int gameID, JeopardyGameInfo& gameInfo);
}

#endif // JEOPARDYDATABASE_H
