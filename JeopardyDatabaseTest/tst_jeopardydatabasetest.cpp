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
    runTestSAFS(";;;;", defaultAction, false);
    runTestSAFS(";;;", defaultAction, false);
    runTestSAFS("s:2;;;", defaultAction, false);
    runTestSAFS("s:2;r:0;c:0;msg:", defaultAction, false);
    runTestSAFS("s:2;r:0;c:0;msg:;", defaultAction, false);
    runTestSAFS("s:2;r:7;c:0;msg:", defaultAction, false);
    runTestSAFS("s:200;r:0;c:0;msg:", defaultAction, false);
    runTestSAFS("s:2;r:0;c:-1;msg:", defaultAction, false);

    StateAction result;
    result.column = 0;
    result.row = 0;
    result.state = GameState::BOARD;
    runTestSAFS("s:2;r:0;c:0;m:", result, true);

    result.message = "message";
    runTestSAFS("s:2;r:0;c:0;m:message", result, true);
    runTestSAFS("s:2;r:0;c:0;m:message;b:", result, true);
}

QTEST_APPLESS_MAIN(JeopardyDatabaseTest)

#include "tst_jeopardydatabasetest.moc"
