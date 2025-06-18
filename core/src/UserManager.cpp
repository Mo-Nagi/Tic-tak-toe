#include "UserManager.h"
#include <algorithm>

UserHashTable::UserHashTable() {
    loadUsers();
}

UserHashTable::~UserHashTable() {
    saveUsers();
}

int UserHashTable::hashFunction(const std::string& key) {
    std::hash<std::string> hasher;
    return hasher(key) % BUCKET_SIZE;
}

bool UserHashTable::insertUser(const std::string& username, const std::string& passwordHash) {
    if (userExists(username)) {
        return false;
    }

    int index = hashFunction(username);
    User newUser(username, passwordHash);
    table[index].emplace_back(username, newUser);
    saveUsers();
    return true;
}

bool UserHashTable::authenticateUser(const std::string& username, const std::string& passwordHash) {
    int index = hashFunction(username);

    for (const auto& pair : table[index]) {
        if (pair.first == username && pair.second.passwordHash == passwordHash) {
            return true;
        }
    }
    return false;
}

bool UserHashTable::userExists(const std::string& username) {
    int index = hashFunction(username);

    for (const auto& pair : table[index]) {
        if (pair.first == username) {
            return true;
        }
    }
    return false;
}

void UserHashTable::removeUser(const std::string& username) {
    int index = hashFunction(username);

    auto& bucket = table[index];
    bucket.remove_if([&username](const std::pair<std::string, User>& pair) {
        return pair.first == username;
    });
    saveUsers();
}

User* UserHashTable::getUser(const std::string& username) {
    int index = hashFunction(username);

    for (auto& pair : table[index]) {
        if (pair.first == username) {
            return &pair.second;
        }
    }
    return nullptr;
}

void UserHashTable::updateUser(const std::string& username, const User& user) {
    int index = hashFunction(username);

    for (auto& pair : table[index]) {
        if (pair.first == username) {
            pair.second = user;
            saveUsers();
            return;
        }
    }
}

std::vector<std::string> UserHashTable::getAllUsers() {
    std::vector<std::string> users;
    for (int i = 0; i < BUCKET_SIZE; i++) {
        for (const auto& pair : table[i]) {
            users.push_back(pair.first);
        }
    }
    return users;
}

void UserHashTable::loadUsers() {
    std::ifstream file("users.dat");
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string username, passwordHash;
        int gamesPlayed, gamesWon, gamesLost, gamesTied;

        if (iss >> username >> passwordHash >> gamesPlayed >> gamesWon >> gamesLost >> gamesTied) {
            User user(username, passwordHash);
            user.gamesPlayed = gamesPlayed;
            user.gamesWon = gamesWon;
            user.gamesLost = gamesLost;
            user.gamesTied = gamesTied;

            int index = hashFunction(username);
            table[index].emplace_back(username, user);
        }
    }
    file.close();
}

void UserHashTable::saveUsers() {
    std::ofstream file("users.dat");
    if (!file.is_open()) {
        return;
    }

    for (int i = 0; i < BUCKET_SIZE; i++) {
        for (const auto& pair : table[i]) {
            const User& user = pair.second;
            file << user.username << " " << user.passwordHash << " "
                 << user.gamesPlayed << " " << user.gamesWon << " "
                 << user.gamesLost << " " << user.gamesTied << "\n";
        }
    }
    file.close();
}

void UserHashTable::clear() {
    for (int i = 0; i < BUCKET_SIZE; i++) {
        table[i].clear();
    }
}
