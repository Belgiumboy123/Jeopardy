#ifndef JEOPARDYGAME_H
#define JEOPARDYGAME_H

#include "jeopardydatabase.h"
#include "options.h"

class QStandardItemModel;
class QModelIndex;

class JeopardyGame
{
public:
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

    DatabaseUtils::StaticGameInfo m_staticGameInfo;

    QStandardItemModel* m_model;

    GameMode m_gameMode;

    int m_cluesAnswered;

    int GetRowFromValue( const int value, const GameMode mode) const;
    void LoadRound( const GameMode gameMode );

    NextClueOptions& m_nextClueOptions;
};

#endif // JEOPARDYGAME_H
