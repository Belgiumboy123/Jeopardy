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
};

JeopardyDatabaseTest::JeopardyDatabaseTest()
{
}

bool isValidClueValue(int value )
{
    int rem = value % 200;
    int div = value / 200;
    return rem == 0 && div > 0 && div < 11;
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
            QVERIFY2( isValidClueValue(value.first), msg.toStdString().c_str());
        }
    }
}

void JeopardyDatabaseTest::testCase1()
{
    runTest( 1, 30, 30);

    runTest(2, 26, 29);

    runTest(5, 29, 24);
}

QTEST_APPLESS_MAIN(JeopardyDatabaseTest)

#include "tst_jeopardydatabasetest.moc"
