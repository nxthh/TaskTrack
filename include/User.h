#pragma once
#include <string>

using std::string;

// Using a standard enum can sometimes be easier for file I/O, 
// but enum class is safer. We will stick with enum class.
enum class Role { User, Admin };

class User {
private:
    string username;
    string password;
    string fullName;
    string gender;
    Role   role;

public:
    // Default constructor - initializes strings to empty and role to User
    User() : username(""), password(""), fullName(""), gender(""), role(Role::User) {}

    // Parameterized constructor
    User(const string& u, const string& p, const string& fn,
         const string& g, Role r)
        : username(u), password(p), fullName(fn), gender(g), role(r) {}

    // ── Getters ──────────────────────────────────────────────
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getFullName() const { return fullName; }
    string getGender()   const { return gender; }
    Role   getRole()     const { return role; }
    bool   isAdmin()     const { return role == Role::Admin; }

    // Returns string version for display or file saving
    string getRoleStr() const { 
        return (role == Role::Admin) ? "Admin" : "User"; 
    }

    // ── Setters ──────────────────────────────────────────────
    void setUsername(const string& v) { username = v; }
    void setPassword(const string& v) { password = v; }
    void setFullName(const string& v) { fullName = v; }
    void setGender(const string& v)   { gender = v; }
    void setRole(Role r)              { role = r; }

    /**
     * Static helper to convert string from file back into a Role enum.
     * This is vital for your FileManager.cpp to work.
     */
    static Role stringToRole(const string& str) {
        if (str == "Admin") return Role::Admin;
        return Role::User;
    }
};