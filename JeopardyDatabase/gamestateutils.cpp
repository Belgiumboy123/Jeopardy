#include "gamestateutils.h"

#include <QStringList>
#include <QDebug>

using GameStateUtils::Clues;
using GameStateUtils::ClueInfo;
using GameStateUtils::StateAction;
using GameStateUtils::StateResponse;

namespace
{
    const QString DOLLAR = "$";
}

Clues::Clues(bool doubleJeopardy)
    : m_doubleJeopardy(doubleJeopardy)
{
    m_clues.resize(TOTAL_ROWS*TOTAL_COLS);
    m_invalidClues.resize(TOTAL_COLS);
}

Clues::~Clues() {}

void
Clues::Reset()
{
    m_clues.clear();
    m_invalidClues.clear();
    m_categoryIndices.clear();

    m_clues.resize(TOTAL_ROWS*TOTAL_COLS);
    m_invalidClues.resize(TOTAL_COLS);
}

QString
Clues::GetClueQuestion( int col, int row)
{
    if( IsValidCol(col) && IsValidRow(row) )
    {
        auto& clueInfo = GetClueInfo(col,row);
        if(!clueInfo.answered)
        {
            clueInfo.answered = true;
            return clueInfo.question;
        }
    }

    return "";
}

QString
Clues::GetClueAnswer(int col, int row) const
{
    if( IsValidCol(col) && IsValidRow(row) )
    {
        return GetClueInfo(col,row).answer;
    }

    return "";
}

bool
Clues::HasClueBeenAnswered(int col, int row) const
{
    if( IsValidCol(col) && IsValidRow(row) )
    {
        return GetClueInfo(col,row).answered;
    }

    return true;
}

void
Clues::SetClueAnswered(int col, int row)
{
    if(IsValidCol(col) && IsValidRow(row))
    {
        GetClueInfo(col,row).answered = true;
    }
}

void
Clues::InsertClue(const QString& category, const QString& question, const QString& answer, const int value)
{
    const int categoryIdx = GetCategoryIndex(category);

    if( IsValidClueValue(value))
    {
        const int row = GetRowFromValue(value);
        auto& clueInfo = GetClueInfo( categoryIdx, row );
        clueInfo.answered = false;
        clueInfo.answer = answer;
        clueInfo.question = question;
    }
    else
    {
        Q_ASSERT(categoryIdx < (int)m_invalidClues.size());

        auto& vec = m_invalidClues[categoryIdx];
        ClueInfo clueInfo;
        clueInfo.answered = false;
        clueInfo.answer = answer;
        clueInfo.question = question;
        vec.push_back( clueInfo );
    }
}

void
Clues::AddInvalidClues()
{
    int column = 0;
    for( const auto& vec : m_invalidClues)
    {
        if (!vec.empty())
        {
            const int start = column*TOTAL_ROWS;
            const int end = start + TOTAL_ROWS;
            for( const auto& info : vec)
            {
                for( int i = start; i<end; i++ )
                {
                    if(m_clues[i].answered)
                    {
                        m_clues[i].answered = false;
                        m_clues[i].question = info.question;
                        m_clues[i].answer = info.answer;
                        break;
                    }
                }
            }
        }

        column++;
    }
}

int
Clues::GetCategoryIndex(const QString& category)
{
    const auto& retval =  m_categoryIndices.insert( std::make_pair(category, m_categoryIndices.size()) );

    return retval.first->second;
}

ClueInfo&
Clues::GetClueInfo(const int col, const int row)
{
    Q_ASSERT(IsValidCol(col) && IsValidRow(row));

    return m_clues[col*TOTAL_ROWS+row];
}

const ClueInfo&
Clues::GetClueInfo(const int col, const int row) const
{
    Q_ASSERT(IsValidCol(col) && IsValidRow(row));

    return m_clues[col*TOTAL_ROWS+row];
}

const QString
Clues::GetCategoryHeader(int column) const
{
    for( const auto& category : m_categoryIndices)
    {
        if( category.second == column)
        {
            return category.first;
        }
    }

    return "";
}

const QString
Clues::GetClueText(int col, int row) const
{
    Q_ASSERT(IsValidCol(col) && IsValidRow(row));

    if( !GetClueInfo(col,row).answered)
    {
        return DOLLAR + QString::number((m_doubleJeopardy ? 400 : 200)*(row+1));
    }

    return "";
}

bool
Clues::IsValidClueValue(const int value) const
{
    switch (value)
    {
    case 200:
    case 600:
    case 1000:
        return !m_doubleJeopardy;

    case 800:
    case 400:
        return true;

    case 1200:
    case 1600:
    case 2000:
        return m_doubleJeopardy;

    default:
        return false;
    }
}

int
Clues::GetRowFromValue(const int value) const
{
    Q_ASSERT(IsValidClueValue(value));

    return (value / (m_doubleJeopardy ? 400 : 200) ) - 1;
}

bool
Clues::IsValidCol(const int col) const
{
    return col > -1 && col < TOTAL_COLS;
}

bool
Clues::IsValidRow(const int row) const
{
    return row > -1 && row < TOTAL_ROWS;
}

int
Clues::GetNumberOfUnansweredClues() const
{
    return std::count_if(m_clues.begin(), m_clues.end(), [](const ClueInfo& info){ return !info.answered; });
}

namespace
{
    const QString S_DELIMIT     = ";";
    const QString S_TOKEN       = ":";
    const QString S_STATE       = "s:";
    const QString S_ROW         = "r:";
    const QString S_COLUMN      = "c:";
    const QString S_MSG         = "m:";
    const QString S_CLUES       = "b:";
    const QString S_CATEGORY    = "|";
    const QString S_CLUE        = ",";

    const int state_upper_bound = static_cast<int>(GameStateUtils::GameState::INVALID);

    template<class T>
    void GenerateFromStringImpl(const QString& str, std::pair<bool,T>& pair, QString& clues)
    {
        // This is attempting to parse strings recieved from clients
        // into StateActions.  Ensure 'str' is in valid condition before
        // setting any values.

        QStringList tokens = str.split(S_DELIMIT);
        if( tokens.size() != 4 && tokens.size() != 5)
            return;

        if( !tokens[0].startsWith(S_STATE) || !tokens[1].startsWith(S_ROW) || !tokens[2].startsWith(S_COLUMN) || !tokens[3].startsWith(S_MSG) )
            return;

        bool ok;
        int stateToken = tokens[0].remove(0,2).toInt(&ok);
        if( !ok || stateToken < 0 || stateToken >= state_upper_bound )
            return;

        ok = false;
        int row = tokens[1].remove(0,2).toInt(&ok);
        if( !ok || row < 0 || row >= GameStateUtils::TOTAL_ROWS)
            return;

        ok = false;
        int column = tokens[2].remove(0,2).toInt(&ok);
        if( !ok || column < 0 || column >= GameStateUtils::TOTAL_COLS)
            return;

        const QString& message = tokens[3].remove(0,2);

        //special case for StateResponse
        if( tokens.size() == 5)
        {
            if( !tokens[4].startsWith(S_CLUES))
                return;

            clues = tokens[4].remove(0,2);
        }

        pair.first = true;
        pair.second.state = static_cast<GameStateUtils::GameState>(stateToken);
        pair.second.row = row;
        pair.second.column = column;
        pair.second.message = message;
    }
}

// StateAction

QString
StateAction::ToString() const
{
    return S_STATE + QString::number(static_cast<int>(state)) + S_DELIMIT
            + S_ROW + QString::number(row) + S_DELIMIT
            + S_COLUMN + QString::number(column) + S_DELIMIT
            + S_MSG + message;
}

/*static*/ std::pair<bool,StateAction>
StateAction::GenerateFromString(const QString& str)
{
    QString clues;
    auto pair = std::make_pair(false,StateAction());
    GenerateFromStringImpl(str, pair, clues/*unused*/);
    return pair;
}

// StateResponse

QString
StateResponse::ToString() const
{
    QString str = StateAction::ToString() + S_CLUES;

    if( clues )
    {
        for( int column = 0; column<TOTAL_COLS; column++)
        {
            str += S_CATEGORY + clues->GetCategoryHeader(column);

            for( int row = 0; row<TOTAL_ROWS; row++)
            {
                str += S_CLUE + clues->GetClueText(column, row);
            }
        }
    }

    return str;
}

/*static*/ std::pair<bool,StateResponse>
StateResponse::GenerateFromString(const QString& str)
{
    QString clues;
    auto pair = std::make_pair(false,StateResponse());
    GenerateFromStringImpl(str, pair, clues);
    pair.second.serverClues = clues;
    return pair;
}
