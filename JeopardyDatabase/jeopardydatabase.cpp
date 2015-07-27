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

DatabaseUtils::StaticGameInfo::StaticGameInfo()
{
    clear();
}

void
DatabaseUtils::StaticGameInfo::clear()
{
    totalDoubleClues = 0;
    totalSingleClues = 0;

    finalCategory.clear();
    finalClue.clear();
    finalAnswer.clear();

    singleRoundQuestions.clear();
    doubleRoundQuestions.clear();
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

bool DatabaseUtils::IsValidClueValue( int value, bool dj)
{
    switch (value)
    {
    case 200:
    case 600:
    case 1000:
        return !dj;

    case 800:
    case 400:
        return true;

    case 1200:
    case 1600:
    case 2000:
        return dj;

    default:
        return false;
    }
}

void
FixDailyDoubleValues(DatabaseUtils::RoundQuestions& roundQuestions, bool doubleJeopardy)
{   
    using namespace DatabaseUtils;
    auto isValidClueFunction = [doubleJeopardy](ValuePair vp)
    {
        return !DatabaseUtils::IsValidClueValue(vp.first, doubleJeopardy);
    };

    for( auto& category : roundQuestions )
    {
        CategoryQuestions& clues = category.second;

        const int numInvalid = std::count_if( clues.cbegin(), clues.cend(), isValidClueFunction);

        if( numInvalid == 1 )
        {
            // find missing value
            int newValue = -1;
            for( int i = 1; i<6; i++)
            {
                if( clues.find(i*(doubleJeopardy?400:200)) == clues.end() )
                {
                    newValue = i*(doubleJeopardy?400:200);
                    break;
                }
            }

            auto iter = std::find_if( clues.cbegin(), clues.cend(), isValidClueFunction);
            category.second.emplace(newValue, iter->second);
            category.second.erase(iter);
        }
        else if( numInvalid > 1)
        {
            // Intended copy, so no weird issues happen
            // while iterating
            CategoryQuestions oldQuestions = clues;

            // clear all clues and set new ones
            clues.clear();

            int clueNumber = 1;
            for( const auto& clue : oldQuestions )
            {
                category.second.emplace(clueNumber*(doubleJeopardy ? 400 : 200), clue.second );
                clueNumber++;
            }
        }
    }
}

void
DatabaseUtils::GetGameInfo(const int gameID, StaticGameInfo& info)
{
    // be sure to clear any variables from a previously loaded game
    info.clear();

    QSqlDatabase db = GetDatabase();

    if(db.open())
    {
        DatabaseUtils::CategoryQuestionsVector singleQuestionsDD;
        DatabaseUtils::CategoryQuestionsVector doubleQuestionsDD;

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

            auto clueAnswerPair = std::make_pair(clue,answer);
            auto valuePair = std::make_pair(value,clueAnswerPair);

            // select the correct variables depending on round
            const bool doubleJeopardy = (round == DOUBLE_JEOPARDY);
            RoundQuestions& questions = doubleJeopardy ? info.doubleRoundQuestions : info.singleRoundQuestions;
            int& totalClues = doubleJeopardy ? info.totalDoubleClues : info.totalSingleClues;

            // add the clue
            auto emplace_retVal = questions.emplace( category, CategoryQuestions() );
            if( emplace_retVal.first->second.insert(valuePair).second )
            {   // successfully added the clue
                totalClues++;
            }
            else
            {
                // User bet daily double amount same as a regular clue value
                // so set value here to -1 which fixDailyDoubleValues can handle
                valuePair.first = -1;
                emplace_retVal.first->second.insert(valuePair);
                totalClues++;
            }
         } // end of query

         FixDailyDoubleValues(info.singleRoundQuestions, false/*doubleJeopardy*/);
         FixDailyDoubleValues(info.doubleRoundQuestions, true/*doubleJeopardy*/);
    }

    db.close();
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
