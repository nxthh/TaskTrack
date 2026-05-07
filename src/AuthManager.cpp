#include "AuthManager.h"
#include "FileManager.h"
#include <iostream>
#include <algorithm>
#include <tabulate/table.hpp>

using namespace std;
using namespace tabulate;

// ── helpers ──────────────────────────────────────────────────────────────────

bool AuthManager::usernameExists(const string& u) const {
    for (auto& user : users)
        if (user.getUsername() == u) return true;
    return false;
}

// ── Lifecycle ────────────────────────────────────────────────────────────────

void AuthManager::loadFromFile() {
    users = FileManager::loadUsers();
    if (users.empty()) {
        cout << "  No users found. Creating default admin account.\n";
        users.emplace_back("admin", "admin123", "Administrator", "N/A", Role::Admin);
        FileManager::saveUsers(users);
    }
}

void AuthManager::saveToFile() const {
    FileManager::saveUsers(users);
}

// ── Auth ─────────────────────────────────────────────────────────────────────

bool AuthManager::signup() {
    string username, password, fullName, gender;
    cout << "\n--- Sign Up ---\n";
    cout << "Username: ";
    cin >> username;

    if (usernameExists(username)) {
        cout << "  Username already taken.\n";
        return false;
    }

    cout << "Password: ";
    cin >> password;
    cin.ignore();

    cout << "Full Name: ";
    getline(cin, fullName);

    cout << "Gender (M / F / Other): ";
    cin >> gender;

    // All sign-ups are regular Users — only the seeded admin account is Admin
    users.emplace_back(username, password, fullName, gender, Role::User);
    saveToFile();
    cout << "  Account created! Please log in.\n";
    return true;
}

bool AuthManager::login() {
    string username, password;
    cout << "\n--- Login ---\n";
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    for (auto& u : users) {
        if (u.getUsername() == username && u.getPassword() == password) {
            currentUser = &u;
            cout << "  Welcome, " << u.getFullName()
                 << "! (" << u.getRoleStr() << ")\n";
            return true;
        }
    }
    cout << "  Invalid username or password.\n";
    return false;
}

void AuthManager::logout() {
    if (currentUser)
        cout << "  Goodbye, " << currentUser->getFullName() << "!\n";
    currentUser = nullptr;
}

// ── Admin features ───────────────────────────────────────────────────────────

void AuthManager::viewAllUsers() const {
    Table t;
    t.add_row({"Username", "Full Name", "Gender", "Role"});
    t[0].format()
        .font_style({FontStyle::bold})
        .font_color(Color::cyan);

    for (auto& u : users)
        t.add_row({u.getUsername(), u.getFullName(), u.getGender(), u.getRoleStr()});

    cout << "\n" << t << "\n";
}

void AuthManager::deleteUser(const string& username) {
    if (username == "admin") {
        cout << "  Cannot delete the primary admin account.\n";
        return;
    }
    auto it = find_if(users.begin(), users.end(),
        [&](const User& u){ return u.getUsername() == username; });
    if (it == users.end()) { cout << "  User not found.\n"; return; }
    users.erase(it);
    if (currentUser && currentUser->getUsername() == username)
        currentUser = nullptr;
    saveToFile();
    cout << "  User '" << username << "' deleted.\n";
}

void AuthManager::resetPassword(const string& username) {
    for (auto& u : users) {
        if (u.getUsername() == username) {
            string newPw;
            cout << "  New password for " << username << ": ";
            cin >> newPw;
            u.setPassword(newPw);
            saveToFile();
            cout << "  Password reset.\n";
            return;
        }
    }
    cout << "  User not found.\n";
}

void AuthManager::searchUser(const string& keyword) const {
    Table t;
    t.add_row({"Username", "Full Name", "Gender", "Role"});
    t[0].format()
        .font_style({FontStyle::bold})
        .font_color(Color::cyan);

    bool found = false;
    for (auto& u : users) {
        if (u.getUsername().find(keyword) != string::npos ||
            u.getFullName().find(keyword)  != string::npos) {
            t.add_row({u.getUsername(), u.getFullName(), u.getGender(), u.getRoleStr()});
            found = true;
        }
    }
    if (!found) cout << "  No users found matching '" << keyword << "'.\n";
    else        cout << "\n" << t << "\n";
}

void AuthManager::clearAllUsers() {
    vector<User> admins;
    for (auto& u : users)
        if (u.isAdmin()) { admins.push_back(u); break; }
    users = admins;
    currentUser = users.empty() ? nullptr : &users[0];
    saveToFile();
    cout << "  All non-admin users cleared.\n";
}