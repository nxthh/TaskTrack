#pragma once
#include <vector>
#include <string>
#include "Task.h"
#include "User.h"

class FileManager {
public:
    // ── System Initialization ─────────────────────────────────
    // Fixed: static ensures System.cpp line 370 works
    static void ensureDataDir();

    // ── Task Management ───────────────────────────────────────
    static void saveTasks(const std::vector<Task>& tasks, const std::string& path = "data/tasks.csv");
    static std::vector<Task> loadTasks(const std::string& path = "data/tasks.csv");
    
    // ── Trash Management ──────────────────────────────────────
    static void saveTrash(const std::vector<Task>& trash, const std::string& path = "data/trash.csv");
    static std::vector<Task> loadTrash(const std::string& path = "data/trash.csv");
    
    // ── User Management ───────────────────────────────────────
    static void saveUsers(const std::vector<User>& users, const std::string& path = "data/users.csv");
    static std::vector<User> loadUsers(const std::string& path = "data/users.csv");

    // ── System Tools ──────────────────────────────────────────
    static void backup();
    static void restore();
    static void clearAllTasks();
    static void clearAllUsers();

private:
    static std::string readFile(const std::string& path);
    static void writeFile(const std::string& path, const std::string& content);
    static bool copyFile(const std::string& src, const std::string& dst);
};