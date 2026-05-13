#include "AuthManager.h"
#include "FileManager.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <tabulate/table.hpp>


using namespace std;
using namespace tabulate;

// ── Helpers ──────────────────────────────────────────────────────────────────

bool AuthManager::usernameExists(const string &u) const {
  for (const auto &user : users)
    if (user.getUsername() == u)
      return true;
  return false;
}

// Requirement 1: Full Name validation (letters and spaces only)
static bool isValidFullName(const string &name) {
  if (name.empty())
    return false;
  for (char c : name) {
    if (!isalpha(static_cast<unsigned char>(c)) &&
        !isspace(static_cast<unsigned char>(c)))
      return false;
  }
  return true;
}

// ── Lifecycle ────────────────────────────────────────────────────────────────

void AuthManager::loadFromFile() {
  users = FileManager::loadUsers();
  // Requirement 6: Seed default admin if file is empty
  if (users.empty()) {
    cout << "   [System] No users found. Creating default admin account.\n";
    users.emplace_back("admin", "admin123", "Administrator", "N/A",
                       Role::Admin);
    FileManager::saveUsers(users);
  }
}

void AuthManager::saveToFile() const { FileManager::saveUsers(users); }

// ── Authentication (Requirement 1) ───────────────────────────────────────────

bool AuthManager::signup() {
  string username, password, fullName, gender, roleInput;
  Role selectedRole = Role::User;

  cout << "\n--- Sign Up ---\n";

  // 1. Username Validation
  cout << "   Username (no spaces): ";
  cin >> username;
  if (usernameExists(username)) {
    cout << "   Error: Username already taken.\n";
    return false;
  }

  // 2. Password Validation (Requirement: Min 6 characters)
  cout << "   Password (min 6 chars): ";
  cin >> password;
  if (password.length() < 6) {
    cout << "   Error: Password too short! Must be at least 6 characters.\n";
    return false;
  }

  cin.ignore(1000, '\n'); // Clear buffer for getline

  // 3. Full Name Validation (Requirement: Letters and spaces only)
  cout << "   Full Name: ";
  getline(cin, fullName);
  if (!isValidFullName(fullName)) {
    cout << "   Error: Full name must contain only letters and spaces.\n";
    return false;
  }

  // Replace this section in signup():
  cout << "   Gender (M / F): ";
  cin >> gender;

  // Normalize full words to single letter
  string gLower = gender;
  for (char &c : gLower)
    c = (char)tolower((unsigned char)c);

  if (gLower == "m" || gLower == "male")
    gender = "M";
  else if (gLower == "f" || gLower == "female")
    gender = "F";
  else {
    cout << "   Error: Invalid gender. Enter M, F, male, or female.\n";
    return false;
  }

  users.emplace_back(username, password, fullName, gender, selectedRole);
  saveToFile();
  cout << "   Account created successfully! You can now log in.\n";
  return true;
}

bool AuthManager::login() {
  string username, password;
  cout << "\n--- Login ---\n";
  cout << "   Username: ";
  cin >> username;
  cout << "   Password: ";
  cin >> password;

  for (auto &u : users) {
    if (u.getUsername() == username && u.getPassword() == password) {
      currentUser = &u; // Set session
      cout << "   Welcome back, " << u.getFullName() << "!\n";
      cout << "   Access Level: [" << u.getRoleStr() << "]\n";
      return true;
    }
  }
  cout << "   Error: Invalid credentials.\n";
  return false;
}

void AuthManager::logout() {
  if (currentUser)
    cout << "   Goodbye, " << currentUser->getFullName()
         << ". Session closed.\n";
  currentUser = nullptr;
}

// ── Admin Features (Requirement 4/6) ─────────────────────────────────────────

void AuthManager::viewAllUsers() const {
  if (users.empty()) {
    cout << "   No users registered.\n";
    return;
  }

  Table t;
  t.add_row({"Username", "Full Name", "Gender", "Role"});
  t[0].format().font_style({FontStyle::bold}).font_color(Color::cyan);

  for (const auto &u : users)
    t.add_row(
        {u.getUsername(), u.getFullName(), u.getGender(), u.getRoleStr()});

  cout << "\n" << t << "\n";
}

void AuthManager::deleteUser(const string &username) {
  // Safety check: Don't lock out the system
  if (username == "admin") {
    cout << "   Error: The primary admin account cannot be deleted.\n";
    return;
  }

  auto it = find_if(users.begin(), users.end(),
                    [&](const User &u) { return u.getUsername() == username; });

  if (it == users.end()) {
    cout << "   Error: User '" << username << "' not found.\n";
    return;
  }

  // Logout if deleting self
  if (currentUser && currentUser->getUsername() == username) {
    currentUser = nullptr;
  }

  users.erase(it);
  saveToFile();
  cout << "   User successfully removed from system.\n";
}

void AuthManager::resetPassword(const string &username) {
  for (auto &u : users) {
    if (u.getUsername() == username) {
      string newPw;
      cout << "   New password for " << username << " (min 6 chars): ";
      cin >> newPw;
      if (newPw.length() < 6) {
        cout << "   Error: Reset failed. Password too short.\n";
        return;
      }
      u.setPassword(newPw);
      saveToFile();
      cout << "   Password reset successfully.\n";
      return;
    }
  }
  cout << "   Error: User not found.\n";
}

void AuthManager::searchUser(const string &keyword) const {
  Table t;
  t.add_row({"Username", "Full Name", "Gender", "Role"});
  t[0].format().font_style({FontStyle::bold}).font_color(Color::cyan);

  bool found = false;
  for (const auto &u : users) {
    // Search in both username and full name
    if (u.getUsername().find(keyword) != string::npos ||
        u.getFullName().find(keyword) != string::npos) {
      t.add_row(
          {u.getUsername(), u.getFullName(), u.getGender(), u.getRoleStr()});
      found = true;
    }
  }

  if (!found) {
    cout << "   No users matched keyword: '" << keyword << "'.\n";
  } else {
    cout << "\n" << t << "\n";
  }
}

void AuthManager::clearAllUsers() {
  cout << "   Wiping all non-admin data. Proceed? (y/n): ";
  char confirm;
  cin >> confirm;
  if (confirm == 'y' || confirm == 'Y') {
    // Use erase-remove_if to keep only Admins
    users.erase(remove_if(users.begin(), users.end(),
                          [](const User &u) { return !u.isAdmin(); }),
                users.end());

    saveToFile();
    cout << "   All user records cleared. Admin accounts preserved.\n";
  }
}