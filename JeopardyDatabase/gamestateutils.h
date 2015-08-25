#pragma once

#include <QString>
#include <unordered_map>
#include <vector>

namespace GameStateUtils
{
    const int TOTAL_ROWS = 5;
    const int TOTAL_COLS = 6;

    enum class GameState
    {
        SERVER_OFFLINE = 0,
        SERVER_ONLINE,
        SERVER_START_MENU,
        MENU,
        BOARD_START,
        BOARD,
        CLUE_QUESTION,
        CLUE_ANSWER,
        CLUE_TIMEOUT,
        FINAL_START,
        FINAL_CATEGORY,
        FINAL_CLUE,
        FINAL_ANSWER,
        GAME_OVER,
        PAUSED,
        INVALID // Used for parsing gamestate
    };

    struct ClueInfo
    {
        QString question;
        QString answer;
        bool answered{true};
    };

    // Hashing function for QString to be used in unordered_map
    struct QStringHash
    {
        std::size_t operator() (const QString& s) const
        {
            return std::hash<std::string>()(s.toStdString());
        }
    };

    class Clues
    {
    public:
        Clues(bool doubleJeopardy);
        virtual ~Clues();

        QString GetClueQuestion( int col, int row);
        QString GetClueAnswer(int col, int row) const;
        bool HasClueBeenAnswered(int col, int row) const;

        void InsertClue(const QString& category, const QString& question, const QString& answer, const int value);
        void AddInvalidClues();

        void SetClueAnswered(int col, int row);

        const QString GetCategoryHeader(int column) const;
        const QString GetClueText(int col, int row) const;

        void Reset();

        // NOTE this function is meant for unit test only
        int GetNumberOfUnansweredClues() const;

    private:
        std::vector<ClueInfo> m_clues;

        // Category -> index in m_clues
        std::unordered_map<QString,int, QStringHash> m_categoryIndices;

        // Clues with invalid clue values found adding clues.
        // Calling AddInvalidClues() will add these clues
        // into m_clues with correct clue values.
        std::vector< std::vector<ClueInfo> > m_invalidClues;

        bool m_doubleJeopardy;

        bool IsValidClueValue(const int value) const;
        int GetRowFromValue(const int value) const;

        // this will create category in map if it doesn't exist.
        int GetCategoryIndex(const QString& category);

        ClueInfo& GetClueInfo(const int col, const int row);
        const ClueInfo& GetClueInfo(const int col, const int row) const;

        bool IsValidRow(const int row) const;
        bool IsValidCol(const int col) const;
    };


    struct StateAction
    {
        GameState   state{GameState::INVALID};
        int         row{-1};
        int         column{-1};
        QString     message;

        virtual QString ToString() const;
        StateAction() = default;

        static std::pair<bool,StateAction> GenerateFromString(const QString& str);
    };

    struct StateResponse : public StateAction
    { 
        Clues*      clues{nullptr};
        QString     serverClues;

        virtual QString ToString() const;
        StateResponse() = default;

        static std::pair<bool,StateResponse> GenerateFromString(const QString& str);
    };
}

