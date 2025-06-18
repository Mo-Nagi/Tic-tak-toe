#include <gtest/gtest.h>
#include "UserManager.h"
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>

class UserManagerTest : public ::testing::Test {
protected:
    UserHashTable users;
    
    void SetUp() override {
        users.clear();
        std::remove("users.dat"); // Clean test file
    }
    
    void TearDown() override {
        std::remove("users.dat"); // Clean up after test
    }
};

// === CONSTRUCTOR/DESTRUCTOR TESTS ===
TEST_F(UserManagerTest, ConstructorInitializesEmpty) {
    UserHashTable newTable;
    auto allUsers = newTable.getAllUsers();
    EXPECT_TRUE(allUsers.empty());
}

TEST_F(UserManagerTest, ConstructorLoadsExistingUsers) {
    users.insertUser("test", "hash");
    
    UserHashTable newTable;
    EXPECT_TRUE(newTable.userExists("test"));
}

TEST_F(UserManagerTest, DestructorSavesUsers) {
    {
        UserHashTable tempTable;
        tempTable.insertUser("temp", "hash");
    } // Destructor called here
    
    UserHashTable newTable;
    EXPECT_TRUE(newTable.userExists("temp"));
}

// === INSERT USER TESTS ===
TEST_F(UserManagerTest, InsertUserSuccess) {
    EXPECT_TRUE(users.insertUser("alice", "hash123"));
    EXPECT_TRUE(users.userExists("alice"));
}

TEST_F(UserManagerTest, InsertUserDuplicateFails) {
    users.insertUser("alice", "hash123");
    EXPECT_FALSE(users.insertUser("alice", "hash456"));
}

TEST_F(UserManagerTest, InsertUserEmptyUsername) {
    EXPECT_TRUE(users.insertUser("", "hash"));
    EXPECT_TRUE(users.userExists(""));
}

TEST_F(UserManagerTest, InsertUserEmptyPassword) {
    EXPECT_TRUE(users.insertUser("user", ""));
    EXPECT_TRUE(users.authenticateUser("user", ""));
}

TEST_F(UserManagerTest, InsertUserSpecialCharacters) {
    EXPECT_TRUE(users.insertUser("user@domain.com", "p@ssw0rd!"));
    EXPECT_TRUE(users.userExists("user@domain.com"));
}

TEST_F(UserManagerTest, InsertUserLongUsername) {
    std::string longName(1000, 'a');
    EXPECT_TRUE(users.insertUser(longName, "hash"));
    EXPECT_TRUE(users.userExists(longName));
}

TEST_F(UserManagerTest, InsertUserLongPassword) {
    std::string longHash(1000, 'b');
    EXPECT_TRUE(users.insertUser("user", longHash));
    EXPECT_TRUE(users.authenticateUser("user", longHash));
}

TEST_F(UserManagerTest, InsertUserUnicodeCharacters) {
    EXPECT_TRUE(users.insertUser("用户", "密码"));
    EXPECT_TRUE(users.userExists("用户"));
}

TEST_F(UserManagerTest, InsertMultipleUsers) {
    for(int i = 0; i < 10; ++i) {
        std::string username = "user" + std::to_string(i);
        EXPECT_TRUE(users.insertUser(username, "hash"));
    }
    auto allUsers = users.getAllUsers();
    EXPECT_EQ(allUsers.size(), 10);
}

// === AUTHENTICATE USER TESTS ===
TEST_F(UserManagerTest, AuthenticateValidUser) {
    users.insertUser("bob", "secrethash");
    EXPECT_TRUE(users.authenticateUser("bob", "secrethash"));
}

TEST_F(UserManagerTest, AuthenticateInvalidPassword) {
    users.insertUser("bob", "secrethash");
    EXPECT_FALSE(users.authenticateUser("bob", "wronghash"));
}

TEST_F(UserManagerTest, AuthenticateNonexistentUser) {
    EXPECT_FALSE(users.authenticateUser("ghost", "anyhash"));
}

TEST_F(UserManagerTest, AuthenticateEmptyCredentials) {
    users.insertUser("", "");
    EXPECT_TRUE(users.authenticateUser("", ""));
}

TEST_F(UserManagerTest, AuthenticateCaseSensitive) {
    users.insertUser("User", "Hash");
    EXPECT_FALSE(users.authenticateUser("user", "Hash"));
    EXPECT_FALSE(users.authenticateUser("User", "hash"));
}

TEST_F(UserManagerTest, AuthenticateAfterMultipleInserts) {
    for(int i = 0; i < 5; ++i) {
        users.insertUser("user" + std::to_string(i), "hash" + std::to_string(i));
    }
    
    for(int i = 0; i < 5; ++i) {
        EXPECT_TRUE(users.authenticateUser("user" + std::to_string(i), "hash" + std::to_string(i)));
    }
}

// === USER EXISTS TESTS ===
TEST_F(UserManagerTest, UserExistsTrue) {
    users.insertUser("charlie", "hash");
    EXPECT_TRUE(users.userExists("charlie"));
}

TEST_F(UserManagerTest, UserExistsFalse) {
    EXPECT_FALSE(users.userExists("nobody"));
}

TEST_F(UserManagerTest, UserExistsAfterRemoval) {
    users.insertUser("temp", "hash");
    EXPECT_TRUE(users.userExists("temp"));
    users.removeUser("temp");
    EXPECT_FALSE(users.userExists("temp"));
}

TEST_F(UserManagerTest, UserExistsEmptyUsername) {
    users.insertUser("", "hash");
    EXPECT_TRUE(users.userExists(""));
}

TEST_F(UserManagerTest, UserExistsCaseSensitive) {
    users.insertUser("User", "hash");
    EXPECT_FALSE(users.userExists("user"));
    EXPECT_TRUE(users.userExists("User"));
}

// === REMOVE USER TESTS ===
TEST_F(UserManagerTest, RemoveExistingUser) {
    users.insertUser("toremove", "hash");
    EXPECT_TRUE(users.userExists("toremove"));
    users.removeUser("toremove");
    EXPECT_FALSE(users.userExists("toremove"));
}

TEST_F(UserManagerTest, RemoveNonexistentUser) {
    EXPECT_NO_THROW(users.removeUser("ghost"));
    EXPECT_FALSE(users.userExists("ghost"));
}

TEST_F(UserManagerTest, RemoveUserTwice) {
    users.insertUser("once", "hash");
    users.removeUser("once");
    EXPECT_NO_THROW(users.removeUser("once"));
    EXPECT_FALSE(users.userExists("once"));
}

TEST_F(UserManagerTest, RemoveUserFromMultiple) {
    users.insertUser("keep1", "hash");
    users.insertUser("remove", "hash");
    users.insertUser("keep2", "hash");
    
    users.removeUser("remove");
    
    EXPECT_TRUE(users.userExists("keep1"));
    EXPECT_FALSE(users.userExists("remove"));
    EXPECT_TRUE(users.userExists("keep2"));
}

// === GET USER TESTS ===
TEST_F(UserManagerTest, GetUserReturnsValidPointer) {
    users.insertUser("dave", "hash");
    User* user = users.getUser("dave");
    ASSERT_NE(user, nullptr);
    EXPECT_EQ(user->username, "dave");
    EXPECT_EQ(user->passwordHash, "hash");
}

TEST_F(UserManagerTest, GetUserReturnsNullForNonexistent) {
    User* user = users.getUser("ghost");
    EXPECT_EQ(user, nullptr);
}

TEST_F(UserManagerTest, GetUserAfterRemoval) {
    users.insertUser("temp", "hash");
    users.removeUser("temp");
    User* user = users.getUser("temp");
    EXPECT_EQ(user, nullptr);
}

TEST_F(UserManagerTest, GetUserDefaultValues) {
    users.insertUser("newuser", "hash");
    User* user = users.getUser("newuser");
    ASSERT_NE(user, nullptr);
    EXPECT_EQ(user->gamesPlayed, 0);
    EXPECT_EQ(user->gamesWon, 0);
    EXPECT_EQ(user->gamesLost, 0);
    EXPECT_EQ(user->gamesTied, 0);
}

// === UPDATE USER TESTS ===
TEST_F(UserManagerTest, UpdateUserStatistics) {
    users.insertUser("player", "hash");
    User* user = users.getUser("player");
    ASSERT_NE(user, nullptr);
    
    user->gamesPlayed = 10;
    user->gamesWon = 7;
    user->gamesLost = 2;
    user->gamesTied = 1;
    
    users.updateUser("player", *user);
    
    User* updated = users.getUser("player");
    EXPECT_EQ(updated->gamesPlayed, 10);
    EXPECT_EQ(updated->gamesWon, 7);
    EXPECT_EQ(updated->gamesLost, 2);
    EXPECT_EQ(updated->gamesTied, 1);
}

TEST_F(UserManagerTest, UpdateNonexistentUser) {
    User fakeUser("ghost", "hash");
    EXPECT_NO_THROW(users.updateUser("ghost", fakeUser));
    EXPECT_FALSE(users.userExists("ghost"));
}

TEST_F(UserManagerTest, UpdateUserPassword) {
    users.insertUser("user", "oldhash");
    User* user = users.getUser("user");
    user->passwordHash = "newhash";
    users.updateUser("user", *user);
    
    EXPECT_FALSE(users.authenticateUser("user", "oldhash"));
    EXPECT_TRUE(users.authenticateUser("user", "newhash"));
}

// === GET ALL USERS TESTS ===
TEST_F(UserManagerTest, GetAllUsersEmpty) {
    auto allUsers = users.getAllUsers();
    EXPECT_TRUE(allUsers.empty());
}

TEST_F(UserManagerTest, GetAllUsersReturnsAll) {
    users.insertUser("user1", "hash1");
    users.insertUser("user2", "hash2");
    users.insertUser("user3", "hash3");
    
    auto allUsers = users.getAllUsers();
    EXPECT_EQ(allUsers.size(), 3);
}

TEST_F(UserManagerTest, GetAllUsersAfterRemoval) {
    users.insertUser("keep", "hash");
    users.insertUser("remove", "hash");
    users.removeUser("remove");
    
    auto allUsers = users.getAllUsers();
    EXPECT_EQ(allUsers.size(), 1);
    EXPECT_EQ(allUsers[0], "keep");
}

// === CLEAR TESTS ===
TEST_F(UserManagerTest, ClearRemovesAllUsers) {
    for(int i = 0; i < 5; ++i) {
        users.insertUser("user" + std::to_string(i), "hash");
    }
    
    users.clear();
    auto allUsers = users.getAllUsers();
    EXPECT_TRUE(allUsers.empty());
}

TEST_F(UserManagerTest, ClearEmptyTable) {
    EXPECT_NO_THROW(users.clear());
    auto allUsers = users.getAllUsers();
    EXPECT_TRUE(allUsers.empty());
}

// === HASH FUNCTION TESTS ===
TEST_F(UserManagerTest, HashFunctionDistribution) {
    // Test that hash function distributes users across buckets
    std::vector<std::string> usernames;
    for(int i = 0; i < 100; ++i) {
        std::string username = "user" + std::to_string(i);
        usernames.push_back(username);
        users.insertUser(username, "hash");
    }
    
    // All users should be inserted successfully
    for(const auto& username : usernames) {
        EXPECT_TRUE(users.userExists(username));
    }
}

// === FILE PERSISTENCE TESTS ===
TEST_F(UserManagerTest, SaveUsersCreatesFile) {
    users.insertUser("test", "hash");
    std::ifstream file("users.dat");
    EXPECT_TRUE(file.good());
    file.close();
}

TEST_F(UserManagerTest, LoadUsersFromFile) {
    users.insertUser("persistent", "hash");
    
    UserHashTable newTable;
    EXPECT_TRUE(newTable.userExists("persistent"));
}

TEST_F(UserManagerTest, PersistUserStatistics) {
    users.insertUser("player", "hash");
    User* user = users.getUser("player");
    user->gamesPlayed = 5;
    user->gamesWon = 3;
    users.updateUser("player", *user);
    
    UserHashTable newTable;
    User* loaded = newTable.getUser("player");
    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->gamesPlayed, 5);
    EXPECT_EQ(loaded->gamesWon, 3);
}

// === PERFORMANCE TESTS ===
TEST_F(UserManagerTest, StressTestManyUsers) {
    for(int i = 0; i < 1000; ++i) {
        std::string username = "user" + std::to_string(i);
        EXPECT_TRUE(users.insertUser(username, "hash"));
    }
    
    auto allUsers = users.getAllUsers();
    EXPECT_EQ(allUsers.size(), 1000);
}

TEST_F(UserManagerTest, StressTestAuthentication) {
    for(int i = 0; i < 100; ++i) {
        users.insertUser("user" + std::to_string(i), "hash" + std::to_string(i));
    }
    
    for(int i = 0; i < 100; ++i) {
        EXPECT_TRUE(users.authenticateUser("user" + std::to_string(i), "hash" + std::to_string(i)));
    }
}

// === EDGE CASES ===
TEST_F(UserManagerTest, CollisionHandling) {
    // Test users that might hash to same bucket
    users.insertUser("abc", "hash1");
    users.insertUser("bca", "hash2");
    users.insertUser("cab", "hash3");
    
    EXPECT_TRUE(users.userExists("abc"));
    EXPECT_TRUE(users.userExists("bca"));
    EXPECT_TRUE(users.userExists("cab"));
    
    EXPECT_TRUE(users.authenticateUser("abc", "hash1"));
    EXPECT_TRUE(users.authenticateUser("bca", "hash2"));
    EXPECT_TRUE(users.authenticateUser("cab", "hash3"));
}

TEST_F(UserManagerTest, NullCharacterHandling) {
    std::string usernameWithNull = "user\0hidden";
    EXPECT_TRUE(users.insertUser(usernameWithNull, "hash"));
    EXPECT_TRUE(users.userExists(usernameWithNull));
}

