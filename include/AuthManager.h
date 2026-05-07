#pragma once
#include <vector>
#include "User.h"

using std::string;
using std::vector;

class AuthManager {
private:
    vector<User> users;
    User*        currentUser = nullptr;   // points into users vector

    bool usernameExists(const string& u) const;

public:
    // ── Lifecycle ─────────────────────────────────────────────
    void loadFromFile();
    void saveToFile() const;

    // ── Auth ──────────────────────────────────────────────────
    bool signup();
    bool login();
    void logout();

    // ── Session ───────────────────────────────────────────────
    bool  isLoggedIn() const { return currentUser != nullptr; }
    User* getUser()          { return currentUser; }

    // ── Admin features ────────────────────────────────────────
    void viewAllUsers()                    const;
    void deleteUser(const string& username);
    void resetPassword(const string& username);
    void searchUser(const string& keyword) const;
    void clearAllUsers();

    // ── Utilities ─────────────────────────────────────────────
    int userCount() const { return (int)users.size(); }
};