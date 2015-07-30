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

    void SetNextClueOptions(const NextClueOptions& nextClueOptions);

    GameStateUtils::StateResponse DoStateAction( const GameStateUtils::StateAction action);

private:
    enum GameMode
    {
        GM_NONE = 0,
        GM_SINGLE,
        GM_DOUBLE,
        GM_FINAL
    };

    void LoadRandomGame();
    void LoadGame(const int gameID );
    void LoadRound( const GameMode gameMode );

    QString HandleBoardAction( const int column, const int row );
    QString HandleClueAction( const int column, const int row );
    GameMode HandleAnswerAction();

    const QString& GetFinalCategory() const;
    const QString& GetFinalClue() const;
    const QString& GetFinalAnswer() const;

    std::pair<int,int> GetNextClue(const int column, const int row);

    bool IsClueAvailable(const int column, const int row) const;

private:
    DatabaseUtils::JeopardyGameInfo m_jeopardyGameInfo;

    GameMode m_gameMode;

    int m_cluesAnswered;

    NextClueOptions& m_nextClueOptions;
};

#endif // JEOPARDYGAME_H
