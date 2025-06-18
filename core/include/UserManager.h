#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <functional>

struct User {
    std::string username;
    std::string passwordHash;
    int gamesPlayed;
    int gamesWon;
    int gamesLost;
    int gamesTied;

    User() : gamesPlayed(0), gamesWon(0), gamesLost(0), gamesTied(0) {}
    User(const std::string& user, const std::string& pass)
        : username(user), passwordHash(pass), gamesPlayed(0), gamesWon(0), gamesLost(0), gamesTied(0) {}
};

class UserHashTable {
private:
    static const int BUCKET_SIZE = 100;
    std::list<std::pair<std::string, User>> table[BUCKET_SIZE];

    int hashFunction(const std::string& key);

public:
    UserHashTable();
    ~UserHashTable();

    bool insertUser(const std::string& username, const std::string& passwordHash);
    bool authenticateUser(const std::string& username, const std::string& passwordHash);
    bool userExists(const std::string& username);
    void removeUser(const std::string& username);
    User* getUser(const std::string& username);
    void updateUser(const std::string& username, const User& user);
    std::vector<std::string> getAllUsers();
    void loadUsers();
    void saveUsers();
    void clear();
};

#endif // USERMANAGER_H
