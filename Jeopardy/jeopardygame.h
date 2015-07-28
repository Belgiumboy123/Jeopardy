#ifndef JEOPARDYGAME_H
#define JEOPARDYGAME_H

#include "jeopardydatabase.h"
#include "options.h"

class JeopardyGame
{
public:
    JeopardyGame();
    JeopardyGame(NextClueOptions& nextClueOptions);
    ~JeopardyGame();

    const GameStateUtils::Clues& GetCurrentClues() const;

    void LoadRandomGame();

    QString HandleBoardAction( const int column, const int row );
    QString HandleClueAction( const int column, const int row );

    enum GameMode
    {
        GM_NONE = 0,
        GM_SINGLE,
        GM_DOUBLE,
        GM_FINAL
    };

    GameMode HandleAnswerAction();

    const QString& GetFinalCategory() const;
    const QString& GetFinalClue() const;
    const QString& GetFinalAnswer() const;

    std::pair<int,int> GetNextClue(const int column, const int row);

    void SetNextClueOptions(const NextClueOptions& nextClueOptions);

private:

    void LoadGame(const int gameID );
    void LoadRound( const GameMode gameMode );

    bool IsClueAvailable(const int column, const int row) const;

    DatabaseUtils::JeopardyGameInfo m_jeopardyGameInfo;

    GameMode m_gameMode;

    int m_cluesAnswered;

    NextClueOptions& m_nextClueOptions;
};

#endif // JEOPARDYGAME_H
