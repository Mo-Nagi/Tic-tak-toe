        -:    0:Source:E:/TicTacToe_Clean/core/src/GameStateStack.cpp
        -:    0:Graph:./GameStateStack.gcno
        -:    0:Data:./GameStateStack.gcda
        -:    0:Runs:2
        -:    1:#include "GameStateStack.h"
        -:    2:
      138:    3:GameStateStack::GameStateStack() {}
        -:    4:
      138:    5:GameStateStack::~GameStateStack() {
      138:    6:    clearStack();
      138:    7:}
        -:    8:
    22883:    9:void GameStateStack::pushMove(const Move& move, const std::vector<std::vector<char>>& boardState) {
    22883:   10:    moveStack.push(move);
    22883:   11:    boardStateStack.push(boardState);
    22883:   12:}
        -:   13:
     2688:   14:Move GameStateStack::popMove() {
     2688:   15:    if (!moveStack.empty()) {
     2684:   16:        Move move = moveStack.top();
     2684:   17:        moveStack.pop();
     2684:   18:        return move;
     2684:   19:    }
        4:   20:    return Move(-1, -1, ' ');
        -:   21:}
        -:   22:
     2664:   23:std::vector<std::vector<char>> GameStateStack::popBoardState() {
     2664:   24:    if (!boardStateStack.empty()) {
     2662:   25:        std::vector<std::vector<char>> board = boardStateStack.top();
     2662:   26:        boardStateStack.pop();
     2662:   27:        return board;
     2662:   28:    }
        2:   29:    return std::vector<std::vector<char>>(3, std::vector<char>(3, ' '));
        -:   30:}
        -:   31:
      822:   32:bool GameStateStack::canUndo() {
      822:   33:    return !moveStack.empty() && !boardStateStack.empty();
        -:   34:}
        -:   35:
     4008:   36:void GameStateStack::clearStack() {
    24207:   37:    while (!moveStack.empty()) {
    20199:   38:        moveStack.pop();
        -:   39:    }
    24229:   40:    while (!boardStateStack.empty()) {
    20221:   41:        boardStateStack.pop();
        -:   42:    }
     4008:   43:}
        -:   44:
      164:   45:size_t GameStateStack::size() {
      164:   46:    return moveStack.size();
        -:   47:}
        -:   48:
        8:   49:Move GameStateStack::topMove() {
        8:   50:    if (!moveStack.empty()) {
        6:   51:        return moveStack.top();
        -:   52:    }
        2:   53:    return Move(-1, -1, ' ');
        -:   54:}
        -:   55:
        6:   56:std::vector<std::vector<char>> GameStateStack::topBoardState() {
        6:   57:    if (!boardStateStack.empty()) {
        4:   58:        return boardStateStack.top();
        -:   59:    }
        2:   60:    return std::vector<std::vector<char>>(3, std::vector<char>(3, ' '));
        -:   61:}
