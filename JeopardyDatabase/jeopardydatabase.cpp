#include "jeopardydatabase.h"

#include <qDebug>
#include <QDate>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDir>

#include "CoreFoundation/CFBundle.h"

static QString DB_NAME = "";

void
DatabaseUtils::UseUnitTestDatabasePath()
{
    DB_NAME = "../Jeopardy/Jeopardy.app/Contents/Resources/clues.db";
}

QString
DatabaseUtils::GetFilePathAppResourcesFile(const QString& filename)
{
    CFURLRef appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), filename.toCFString(), NULL, NULL);
    CFStringRef filePathRef = CFURLCopyPath(appUrlRef);
    const char* filePath = CFStringGetCStringPtr(filePathRef, kCFStringEncodingUTF8);
    const QString filePathFinal = QString(filePath);

    // Release references
    CFRelease(filePathRef);
    CFRelease(appUrlRef);

    return filePathFinal;
}

namespace
{
    const QChar COMMA = ',';

    // the following function is in place to
    // avoid QSqlDatabasePrivate::addDatabase: duplicate connection name
    // warning messages by just adding the datasource once
    QSqlDatabase GetDatabase()
    {
        // TODO could we use std::call_once and std::once_flag here instead
        static bool OPENED = false;
        if( !OPENED)
        {
            if( DB_NAME.isEmpty())
            {
                DB_NAME = DatabaseUtils::GetFilePathAppResourcesFile("clues.db");
            }

            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName( DB_NAME);
            OPENED = true;
        }

        return QSqlDatabase::database();
    }
}

int
DatabaseUtils::GetANonPlayedGame()
{
    QSqlDatabase db = GetDatabase();

    int gameID = -1;
    if( db.open())
    {
        QSqlQuery query("SELECT game,lastplayed FROM airdates");
        while (query.next())
        {
            const QString& lastplayed = query.value(1).toString();
            if( lastplayed.isEmpty() )
            {
                gameID = query.value(0).toInt();
                break;
            }
        }
    }

    if( gameID > -1 )
    {
        // mark this game as played
        const QDate today = QDate::currentDate();
        QString sDate = "";
        sDate += QString::number(today.year()) + "-";
        sDate += QString::number(today.month()) + "-";
        sDate += QString::number(today.day());

        const QString sQuery = "UPDATE airdates SET lastplayed=? WHERE game=?;";
        QSqlQuery query;
        query.prepare(sQuery);
        query.bindValue(0, sDate);
        query.bindValue(1, gameID);
        query.exec();
    }

    db.close();

    return 5;//gameID;
}

DatabaseUtils::JeopardyGameInfo::JeopardyGameInfo()
{
    clear();
}

void
DatabaseUtils::JeopardyGameInfo::clear()
{
    totalDoubleClues = 0;
    totalSingleClues = 0;

    finalCategory.clear();
    finalClue.clear();
    finalAnswer.clear();

    singleRoundClues.Reset();
    doubleRoundClues.Reset();
}

namespace
{
    // string used by QSqlQuery needs to be on one line
    const QString sQuery = "SELECT clues.id, clues.game, round, value, category, clue, answer FROM clues JOIN documents ON clues.id = documents.id JOIN classifications ON clues.id = classifications.clue_id JOIN categories ON classifications.category_id = categories.id WHERE clues.game = ?";

    // this directly corresponds to the above query string
    enum QueryResultOrder
    {
        QRO_Round = 2,
        QRO_Value = 3,
        QRO_Category = 4,
        QRO_Clue = 5,
        QRO_Answer = 6
    };

    // This is how database stores round values
    const int FINAL_JEOPARDY = 3;
    const int DOUBLE_JEOPARDY = 2;
    //const int SINGLE_JEOPARDY = 1; // unused
}

void
DatabaseUtils::GetJeopardyGameInfo(const int gameID, JeopardyGameInfo& info)
{
    // be sure to clear any variables from a previously loaded game
    info.clear();

    QSqlDatabase db = GetDatabase();

    if(db.open())
    {
         QSqlQuery query(db);
         query.setForwardOnly(true);
         query.prepare(sQuery);
         query.addBindValue(gameID);
         query.exec();
         while(query.next())
         {
            const int& round = query.value(QRO_Round).toInt();

            if( round == FINAL_JEOPARDY )
            {
                info.finalClue = query.value(QRO_Clue).toString();
                info.finalAnswer = query.value(QRO_Answer).toString();
                info.finalCategory = query.value(QRO_Category).toString();
                continue;
            }

            // database numbers have comma's in them which Qt number conversion can't handle
            // so remove comma's from numbers first
            const int& value = query.value(QRO_Value).toString().remove(COMMA).toInt();
            const QString& category = query.value(QRO_Category).toString();
            const QString& clue = query.value(QRO_Clue).toString().toUpper();
            const QString& answer = query.value(QRO_Answer).toString().toUpper();

            // select the correct variables depending on round
            const bool doubleJeopardy = (round == DOUBLE_JEOPARDY);
            auto& clues = doubleJeopardy ? info.doubleRoundClues : info.singleRoundClues;
            int& totalClues = doubleJeopardy ? info.totalDoubleClues : info.totalSingleClues;
            totalClues++;

            clues.InsertClue(category, clue, answer, value);
         }

         // Add any invalid clues that were found during insertion
         info.singleRoundClues.AddInvalidClues();
         info.doubleRoundClues.AddInvalidClues();
    }

    db.close();
}
