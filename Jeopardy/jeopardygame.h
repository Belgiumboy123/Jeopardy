#ifndef JEOPARDYGAME_H
#define JEOPARDYGAME_H

#include "jeopardydatabase.h"
#include "options.h"

class QStandardItemModel;
class QModelIndex;

class JeopardyGame
{
public:
    JeopardyGame();
    JeopardyGame(NextClueOptions& nextClueOptions);
    ~JeopardyGame();

    QStandardItemModel* GetModel() const;

    void LoadRandomGame();

    QString HandleBoardAction( const QModelIndex& index );
    QString HandleClueAction( const QModelIndex& index );

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

    QModelIndex GetNextClue(const QModelIndex& currentClue);

    void SetNextClueOptions(const NextClueOptions& nextClueOptions);

private:

    void LoadGame(const int gameID );
    void LoadRound( const GameMode gameMode );

    int GetRowFromValue( const int value, const GameMode mode) const;

    DatabaseUtils::StaticGameInfo m_staticGameInfo;
    DatabaseUtils::JeopardyGameInfo m_jeopardyGameInfo;

    QStandardItemModel* m_model;

    GameMode m_gameMode;

    int m_cluesAnswered;

    NextClueOptions& m_nextClueOptions;
};

#endif // JEOPARDYGAME_H
