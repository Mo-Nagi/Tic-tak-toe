#ifndef GAMESTATESTACK_H
#define GAMESTATESTACK_H

#include "GameHistory.h"
#include <stack>
#include <vector>

class GameStateStack {
private:
    std::stack<Move> moveStack;
    std::stack<std::vector<std::vector<char>>> boardStateStack;

public:
    GameStateStack();
    ~GameStateStack();

    void pushMove(const Move& move, const std::vector<std::vector<char>>& boardState);
    Move popMove();
    std::vector<std::vector<char>> popBoardState();
    bool canUndo();
    void clearStack();
    size_t size();
    Move topMove();
    std::vector<std::vector<char>> topBoardState();
};

#endif // GAMESTATESTACK_H
