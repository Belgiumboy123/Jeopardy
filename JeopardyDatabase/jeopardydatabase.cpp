#include "jeopardydatabase.h"

#include <qDebug>
#include <QDate>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

static const QString DB_NAME = "/Users/brechtmoulin/Documents/jeo/jeopardy-parser/clues.db";

namespace
{
    bool IsInteger(const double& d)
    {
        return d - (double)(int)d == 0.0;
    }

    const QChar COMMA = ',';

    // the following function is in palce to
    // avoid QSqlDatabasePrivate::addDatabase: duplicate connection name
    // warning messages by just adding the datasource once
    QSqlDatabase GetDatabase()
    {
        static bool OPENED = false;
        if( !OPENED)
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName( DB_NAME );
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

    return gameID;
}

DatabaseUtils::StaticGameInfo::StaticGameInfo()
    : totalSingleClues(0)
    , totalDoubleClues(0)
{
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

    for( auto& category : roundQuestions )
    {
        CategoryQuestions& clues = category.second;
        auto isValidClueFunction = [doubleJeopardy](ValuePair vp)
        {
            return !DatabaseUtils::IsValidClueValue(vp.first, doubleJeopardy);
        };

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

        // add empty clues for missing values
    }
}

/**
 * @brief FixDailyDoubleValues
 * @param roundQuestions
 *
 * The database stores daily double values as the value
 * that the player happened to bet that game
 * so go through rest of clues in category and figure out
 * the daily double board value
 */
void
FixDailyDoubleValues_old( DatabaseUtils::RoundQuestions& roundQuestions, bool doubleJeopardy)
{
    DatabaseUtils::CategoryQuestions::iterator dailyDoubleIter;
    bool dailyDoubleFound = false;
    for( auto& category : roundQuestions )
    {
        std::vector<bool> validClues(5,false);
        dailyDoubleFound = false;
        for( auto clue : category.second)
        {
             double value = clue.first / (doubleJeopardy ? 400 : 200);
             if( !IsInteger(value) || value > 6 || value < 1)
             {
                   // we have found daily double
                   //qDebug() << clue.first << " " << category.first << " " << value;
                   dailyDoubleIter = category.second.find(clue.first);
                   dailyDoubleFound = true;
             }
             else
             {
                 validClues[value-1] = true;
             }
        }

        // if multiple mistakes were found
        // sort and assign correct values

        if( dailyDoubleFound )
        {
           for( int i = 0; i<5; i++)
           {
               if( !validClues[i] )
               {
                   int value = (i+1)*(doubleJeopardy ? 400 : 200);
                   category.second.emplace( value, dailyDoubleIter->second);
                   category.second.erase( dailyDoubleIter );
                   validClues[i] = true;
                   break;
               }
           }
        }

        // add empty clues for clues that were never asked
        for( int i = 0; i<5; i++)
        {
           if( !validClues[i])
           {
                const int value = (i+1)*(doubleJeopardy ? 400 : 200);
                category.second.emplace( value, std::make_pair("",""));
           }
        }
    }
}

void
DatabaseUtils::GetGameInfo(const int gameID, StaticGameInfo& info)
{
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
