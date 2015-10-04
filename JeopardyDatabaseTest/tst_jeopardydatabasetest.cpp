#include <QString>
#include <QtTest>
#include <QDebug>

#include "jeopardydatabase.h"

using namespace GameStateUtils;

class JeopardyDatabaseTest : public QObject
{
    Q_OBJECT

public:
    JeopardyDatabaseTest();

private Q_SLOTS:
    void testLoadGame();
    void testStateActionFromString();
    void testStateResponseFromString();
    void testParseResponseServerClues();
    void testGetEditDistance();
};

JeopardyDatabaseTest::JeopardyDatabaseTest()
{
}

void runLoadGameTest(int gameID, int totalSingleClues, int totalDoubleClues)
{
    DatabaseUtils::JeopardyGameInfo gameInfo;
    DatabaseUtils::GetJeopardyGameInfo(gameID, gameInfo);

    QVERIFY2(!gameInfo.finalCategory.isEmpty(), "Final Category is empty");
    QVERIFY2(!gameInfo.finalClue.isEmpty(), "Final question is empty");
    QVERIFY2(!gameInfo.finalAnswer.isEmpty(), "Final answer is empty");

    QVERIFY2(gameInfo.totalSingleClues == totalSingleClues, "Wrong number of double round Qs");
    QVERIFY2(gameInfo.totalDoubleClues == totalDoubleClues, "Wrong number of single round Qs");

    const int numUnanswered = gameInfo.singleRoundClues.GetNumberOfUnansweredClues();
    QString msg = QString("Unanswered single clues ") + QString::number(numUnanswered);
    QVERIFY2(numUnanswered == totalSingleClues, msg.toStdString().c_str());

    const int numUnanswered2 = gameInfo.doubleRoundClues.GetNumberOfUnansweredClues();
    msg = QString("Unanswered double clues ") + QString::number(numUnanswered2);
    QVERIFY2(numUnanswered2 == totalDoubleClues, msg.toStdString().c_str());
}

void JeopardyDatabaseTest::testLoadGame()
{
    DatabaseUtils::UseUnitTestDatabasePath();

    runLoadGameTest(1, 30, 30);

    runLoadGameTest(2, 26, 29);

    runLoadGameTest(5, 29, 24);
}

void runTestSAFS(const QString& str, const StateAction& expectedAction, const bool successParse)
{
    auto pair = StateAction::GenerateFromString(str);
    const auto& actualAction = pair.second;

    QVERIFY(pair.first == successParse);
    QVERIFY(actualAction.state == expectedAction.state);
    QVERIFY(actualAction.row == expectedAction.row);
    QVERIFY(actualAction.column == expectedAction.column);
    QVERIFY(actualAction.message == expectedAction.message);
}

void JeopardyDatabaseTest::testStateActionFromString()
{
    StateAction defaultAction;

    runTestSAFS("", defaultAction, false);
    runTestSAFS("^^^^^^^^", defaultAction, false);
    runTestSAFS("^^^^^^", defaultAction, false);
    runTestSAFS("s:2^^^^^^", defaultAction, false);
    runTestSAFS("s:2^^r:0^^c:0^^msg:", defaultAction, false);
    runTestSAFS("s:2^^r:0^^c:0^^m^^", defaultAction, false);
    runTestSAFS("s:2^^r:7^^c:0^^m:", defaultAction, false);
    runTestSAFS("s:200^^r:0^^c:0^^m:", defaultAction, false);
    runTestSAFS("s:2^^r:0^^c:-1^^m:", defaultAction, false);
    runTestSAFS("s:5^^r:0^^c:0^^m:message^^b:", defaultAction, false);

    StateAction result;
    result.column = 0;
    result.row = 0;
    result.state = GameState::BOARD_START;
    runTestSAFS("s:5^^r:0^^c:0^^m:", result, true);

    result.message = "message";
    runTestSAFS("s:5^^r:0^^c:0^^m:message", result, true);
}

void runTestSRFS(const QString& str, const StateResponse& expectedAction, const bool successParse)
{
    auto pair = StateResponse::GenerateFromString(str);
    const auto& actualResponse = pair.second;

    QVERIFY(pair.first == successParse);
    QVERIFY(actualResponse.state == expectedAction.state);
    QVERIFY(actualResponse.row == expectedAction.row);
    QVERIFY(actualResponse.column == expectedAction.column);
    QVERIFY(actualResponse.message == expectedAction.message);
}

void JeopardyDatabaseTest::testStateResponseFromString()
{
    StateResponse result;
    result.state = GameState::SERVER_START_MENU;
    result.row = -1;
    result.column = -1;
    runTestSRFS("s:2^^r:-1^^c:-1^^m:^^b:", result, true);

    StateResponse defaultResponse;
    runTestSRFS("s:2^^r:-1^^c:-1^^m:b:", defaultResponse, false);
    runTestSRFS("s:2^^r:-1^^c:2^^m:^^b:", defaultResponse, false);
    runTestSRFS("s:2^^r:-8^^c:-1;m:^^b:", defaultResponse, false);

    // The clue answer has a ; (semicolon) in it. Which was being used as the delimeter
    // So obviously this wasn't being parsed correctly
    const QString message = "s:7^^r:4^^c:4^^m:IT'S FUN TO RIDE THE AERIAL TRAMWAY FROM MANHATTAN TO THIS EAST RIVER ISLAND; CATCH IT AT 2ND AVENUE & 59TH STREET^^b:";
    result.state = GameState::CLUE_QUESTION;
    result.row = 4;
    result.column = 4;
    result.message = "IT'S FUN TO RIDE THE AERIAL TRAMWAY FROM MANHATTAN TO THIS EAST RIVER ISLAND; CATCH IT AT 2ND AVENUE & 59TH STREET";
    runTestSRFS(message, result, true);
}

void runTestPRST(QString serverClues, bool success)
{
    bool parseTotalSuccess = true;

    for( int column = 0; column<TOTAL_COLS; column++)
    {
        auto result = GetCategoryHeader(serverClues);
        parseTotalSuccess = parseTotalSuccess && result.first;

        for( int row = 0; row<TOTAL_ROWS; row++)
        {
            auto result = GetClueText(serverClues);
            parseTotalSuccess = parseTotalSuccess && result.first;
        }
    }

    QVERIFY(success == parseTotalSuccess);
}

void JeopardyDatabaseTest::testParseResponseServerClues()
{
    runTestPRST("", false);
    runTestPRST("|THE HISTORY CHANNEL,$200,$400,$600,$800,|AD CAMPAIGNS,$200,$400,$600,$800,$1000|KNOW YOUR MUPPETS,$200,$400,$600,$800,$1000|PRESIDENTIAL LIFETIMES,$200,$400,$600,$800,$1000|UNIVERSITY OF MICHIGAN ALUMNI,$200,$400,$600,$800,$1000|ENDS IN \"X\",$200,$400,$600,$800,$1000", true);
}

void runEditDistanceTest(const QString& str1, const QString& str2, const int expectedEditDistance)
{
    const int actualEditDistance = GameStateUtils::GetEditDistance(str1, str2);
    QVERIFY(actualEditDistance == expectedEditDistance);
}

void JeopardyDatabaseTest::testGetEditDistance()
{
    runEditDistanceTest("","", 0);
    runEditDistanceTest("", "test", 4);
    runEditDistanceTest("test", "", 4);

    runEditDistanceTest("test", "best", 1);
}

QTEST_APPLESS_MAIN(JeopardyDatabaseTest)

#include "tst_jeopardydatabasetest.moc"
