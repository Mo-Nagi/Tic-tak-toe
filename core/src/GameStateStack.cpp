#include "GameStateStack.h"

GameStateStack::GameStateStack() {}

GameStateStack::~GameStateStack() {
    clearStack();
}

void GameStateStack::pushMove(const Move& move, const std::vector<std::vector<char>>& boardState) {
    moveStack.push(move);
    boardStateStack.push(boardState);
}

Move GameStateStack::popMove() {
    if (!moveStack.empty()) {
        Move move = moveStack.top();
        moveStack.pop();
        return move;
    }
    return Move(-1, -1, ' ');
}

std::vector<std::vector<char>> GameStateStack::popBoardState() {
    if (!boardStateStack.empty()) {
        std::vector<std::vector<char>> board = boardStateStack.top();
        boardStateStack.pop();
        return board;
    }
    return std::vector<std::vector<char>>(3, std::vector<char>(3, ' '));
}

bool GameStateStack::canUndo() {
    return !moveStack.empty() && !boardStateStack.empty();
}

void GameStateStack::clearStack() {
    while (!moveStack.empty()) {
        moveStack.pop();
    }
    while (!boardStateStack.empty()) {
        boardStateStack.pop();
    }
}

size_t GameStateStack::size() {
    return moveStack.size();
}

Move GameStateStack::topMove() {
    if (!moveStack.empty()) {
        return moveStack.top();
    }
    return Move(-1, -1, ' ');
}

std::vector<std::vector<char>> GameStateStack::topBoardState() {
    if (!boardStateStack.empty()) {
        return boardStateStack.top();
    }
    return std::vector<std::vector<char>>(3, std::vector<char>(3, ' '));
}
