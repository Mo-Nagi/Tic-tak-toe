#include "GameHistory.h"

GameHistory::GameHistory() : historyFile("game_history.dat") {
    loadHistory();
}

void GameHistory::addGameRecord(const GameRecord& record) {
    gameRecords.push_back(record);
    saveHistory();
}

std::vector<GameRecord> GameHistory::getUserGames(const std::string& username) {
    std::vector<GameRecord> userGames;
    for (const auto& record : gameRecords) {
        if (record.player1 == username || record.player2 == username) {
            userGames.push_back(record);
        }
    }
    return userGames;
}

std::vector<GameRecord> GameHistory::getAllGames() {
    return gameRecords;
}

void GameHistory::saveHistory() {
    std::ofstream file(historyFile);
    if (!file.is_open()) {
        return;
    }

    for (const auto& record : gameRecords) {
        file << record.player1 << "|" << record.player2 << "|"
             << static_cast<int>(record.mode) << "|" << static_cast<int>(record.result) << "|"
             << record.timestamp << "|";

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                file << record.finalBoard[i][j];
            }
        }
        file << "|";

        for (const auto& move : record.moves) {
            file << move.row << "," << move.col << "," << move.player << ";";
        }
        file << "\n";
    }

    file.close();
}

void GameHistory::loadHistory() {
    std::ifstream file(historyFile);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(iss, token, '|')) {
            tokens.push_back(token);
        }

        if (tokens.size() >= 6) {
            GameRecord record;
            record.player1 = tokens[0];
            record.player2 = tokens[1];
            record.mode = static_cast<GameMode>(std::stoi(tokens[2]));
            record.result = static_cast<GameResult>(std::stoi(tokens[3]));
            record.timestamp = tokens[4];

            std::string boardStr = tokens[5];
            record.finalBoard.resize(3, std::vector<char>(3));
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    record.finalBoard[i][j] = boardStr[i * 3 + j];
                }
            }

            if (tokens.size() > 6) {
                std::string movesStr = tokens[6];
                std::istringstream moveStream(movesStr);
                std::string moveToken;

                while (std::getline(moveStream, moveToken, ';')) {
                    if (!moveToken.empty()) {
                        std::istringstream moveDetails(moveToken);
                        std::string detail;
                        std::vector<std::string> moveData;

                        while (std::getline(moveDetails, detail, ',')) {
                            moveData.push_back(detail);
                        }

                        if (moveData.size() == 3) {
                            Move move(std::stoi(moveData[0]), std::stoi(moveData[1]), moveData[2][0]);
                            record.moves.push_back(move);
                        }
                    }
                }
            }

            gameRecords.push_back(record);
        }
    }

    file.close();
}

std::string GameHistory::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void GameHistory::loadHistoryIfNeeded() {
    if (gameRecords.empty()) {
        loadHistory();
    }
}
