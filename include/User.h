#pragma once
#include <string>
using namespace std;

enum class Role { User, Admin };

class User {
private:
    string username;
    string password;    // stored as-is; swap with a hash function if desired
    string fullName;
    string gender;
    Role   role;

public:
    User() : role(Role::User) {}
    User(const string& u, const string& p, const string& fn,
         const string& g, Role r)
        : username(u), password(p), fullName(fn), gender(g), role(r) {}

    // ── getters ──────────────────────────────────────────────
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getFullName() const { return fullName; }
    string getGender()   const { return gender; }
    Role   getRole()     const { return role; }
    bool   isAdmin()     const { return role == Role::Admin; }

    // ── setters ──────────────────────────────────────────────
    void setUsername(const string& v) { username = v; }
    void setPassword(const string& v) { password = v; }
    void setFullName(const string& v) { fullName = v; }
    void setGender(const string& v)   { gender = v; }
    void setRole(Role r)              { role = r; }

    string getRoleStr() const { return role == Role::Admin ? "Admin" : "User"; }
};
