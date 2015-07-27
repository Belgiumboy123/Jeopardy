#include <QString>
#include <QtTest>

#include "jeopardydatabase.h"


class JeopardyDatabaseTest : public QObject
{
    Q_OBJECT

public:
    JeopardyDatabaseTest();

private Q_SLOTS:
    void testCase1();
    void testCase2();
};

JeopardyDatabaseTest::JeopardyDatabaseTest()
{
}

void runTest(int gameID, int totalSingleClues, int totalDoubleClues)
{
    DatabaseUtils::StaticGameInfo gameInfo;
    DatabaseUtils::GetGameInfo(gameID, gameInfo);

    QVERIFY2(!gameInfo.finalCategory.isEmpty(), "Final Category is empty");
    QVERIFY2(!gameInfo.finalClue.isEmpty(), "Final question is empty");
    QVERIFY2(!gameInfo.finalAnswer.isEmpty(), "Final answer is empty");

    QVERIFY2(gameInfo.totalSingleClues == totalSingleClues, "Wrong number of double round Qs");
    QVERIFY2(gameInfo.totalDoubleClues == totalDoubleClues, "Wrong number of single round Qs");

    for( const auto& category : gameInfo.singleRoundQuestions)
    {
        for( const auto& value : category.second)
        {
            QString msg = QString("Invalid clue value ") + QString::number(value.first);
            QVERIFY2( DatabaseUtils::IsValidClueValue(value.first, false/*dj*/), msg.toStdString().c_str());
        }
    }

    for( const auto& category : gameInfo.doubleRoundQuestions)
    {
        for( const auto& value : category.second)
        {
            QString msg = QString("Invalid clue value ") + QString::number(value.first);
            QVERIFY2( DatabaseUtils::IsValidClueValue(value.first, true/*dj*/), msg.toStdString().c_str());
        }
    }
}

void JeopardyDatabaseTest::testCase1()
{
    DatabaseUtils::UseUnitTestDatabasePath();

    runTest(1, 30, 30);

    runTest(2, 26, 29);

    runTest(5, 29, 24);
}

void runTest2(int gameID, int totalSingleClues, int totalDoubleClues)
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

void JeopardyDatabaseTest::testCase2()
{
    DatabaseUtils::UseUnitTestDatabasePath();

    runTest2(1, 30, 30);

    runTest2(2, 26, 29);

    runTest2(5, 29, 24);
}

QTEST_APPLESS_MAIN(JeopardyDatabaseTest)

#include "tst_jeopardydatabasetest.moc"
