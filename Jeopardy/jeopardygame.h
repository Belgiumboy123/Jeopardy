#ifndef JEOPARDYGAME_H
#define JEOPARDYGAME_H

#include "jeopardydatabase.h"


class QStandardItemModel;
class QModelIndex;

class JeopardyGame
{
public:
    JeopardyGame();
    ~JeopardyGame();

    QStandardItemModel* GetModel() const;

    void LoadRandomGame();

    QString HandleBoardAction( const QModelIndex& index );
    QString HandleClueAction( const QModelIndex& index );
    bool HandleAnswerAction();

    const QString& GetFinalCategory() const;
    const QString& GetFinalClue() const;
    const QString& GetFinalAnswer() const;

    QModelIndex GetNextClue();

private:

    void LoadGame(const int gameID );

    DatabaseUtils::StaticGameInfo m_staticGameInfo;

    QStandardItemModel* m_model;

    enum GameMode
    {
        GM_NONE = 0,
        GM_SINGLE,
        GM_DOUBLE,
        GM_FINAL
    };

    GameMode m_gameMode;

    int m_cluesAnswered;

    int GetRowFromValue( const int value, const GameMode mode) const;
    void LoadRound( const GameMode gameMode );
};

#endif // JEOPARDYGAME_H
