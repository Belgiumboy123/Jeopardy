#pragma once

#include <QString>
#include <unordered_map>
#include <vector>
#include <map>

namespace GameStateUtils
{
    const int TOTAL_ROWS = 5;
    const int TOTAL_COLS = 6;

    enum class GameState
    {
        SERVER_OFFLINE = 0,
        SERVER_ONLINE,
        SERVER_GAME_PICK,
        SERVER_GAME_JEOPARDY,
        SERVER_GAME_BATTLESHIP,
        SERVER_START_MENU,
        SERVER_GAME_START,
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
        OPPONENT_DISCONNECTED,
        INVALID // Used for parsing gamestate
    };

    static std::map<GameState,QString> GameStateString
    {       {GameState::SERVER_OFFLINE,         "Server offline"}
        ,   {GameState::SERVER_ONLINE,          "SERVER_ONLINE"}
        ,   {GameState::SERVER_START_MENU,      "SERVER_START_MENU"}
        ,   {GameState::SERVER_GAME_START,      "SERVER_GAME_START"}
        ,   {GameState::MENU,                   "MENU"}
        ,   {GameState::BOARD,                  "BOARD"}
        ,   {GameState::BOARD_START,            "BOARD_START"}
        ,   {GameState::CLUE_QUESTION,          "CLUE_QUESTION"}
        ,   {GameState::CLUE_ANSWER,            "CLUE_ANSWER"}
        ,   {GameState::CLUE_TIMEOUT,           "CLUE_TIMEOUT"}
        ,   {GameState::FINAL_START,            "FINAL_START"}
        ,   {GameState::FINAL_CATEGORY,         "FINAL_CATEGORY"}
        ,   {GameState::FINAL_CLUE,             "FINAL_CLUE"}
        ,   {GameState::FINAL_ANSWER,           "FINAL_ANSWER"}
        ,   {GameState::GAME_OVER,              "GAME_OVER"}
        ,   {GameState::INVALID,                "INVALID"}
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

    std::pair<bool,QString> GetCategoryHeader(QString& clues);
    std::pair<bool,QString> GetClueText(QString& clues);

    int GetEditDistance(const QString& str1, const QString str2);
}

