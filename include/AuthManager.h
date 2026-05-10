#pragma once
#include <vector>
#include <string>
#include "User.h"

class AuthManager {
private:
    std::vector<User> users;
    User* currentUser = nullptr; // Points to the logged-in user in the vector

    // Helper to verify uniqueness (Requirement 1)
    bool usernameExists(const std::string& u) const;

public:
    // ── Lifecycle ───────────────────────────────────────────────────────────
    // Requirement 6: Load data automatically on start
    void loadFromFile();
    void saveToFile() const;

    // ── Authentication (Requirement 1) ──────────────────────────────────────
    bool signup();  // Includes validation for name, password, and role
    bool login();   // Verify credentials and set currentUser
    void logout();  // Clear session

    // ── Session Management ──────────────────────────────────────────────────
    bool  isLoggedIn() const { return currentUser != nullptr; }
    
    // Returns the currently logged-in user object
    User* getUser()          { return currentUser; }

    // ── Admin Features (Requirement 4/6) ────────────────────────────────────
    void viewAllUsers() const;
    void deleteUser(const std::string& username);
    void resetPassword(const std::string& username);
    void searchUser(const std::string& keyword) const;
    
    // Clears all non-admin users from the database
    void clearAllUsers();

    // ── Utilities ───────────────────────────────────────────────────────────
    int userCount() const { return (int)users.size(); }
};