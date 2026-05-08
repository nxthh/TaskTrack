#pragma once
#include <vector>
#include <string>
#include "Task.h"
#include "User.h"

using std::vector;
using std::string;

class FileManager {
public:
    // ── Tasks ─────────────────────────────────────────────────────
    static void         saveTasks(const vector<Task>& tasks,
                                  const string& path = "data/tasks.csv");
    static vector<Task> loadTasks(const string& path = "data/tasks.csv");

    // ── Trash (soft-deleted tasks) ────────────────────────────────
    static void         saveTrash(const vector<Task>& trash,
                                  const string& path = "data/trash.csv");
    static vector<Task> loadTrash(const string& path = "data/trash.csv");

    // ── Users ─────────────────────────────────────────────────────
    static void         saveUsers(const vector<User>& users,
                                  const string& path = "data/users.csv");
    static vector<User> loadUsers(const string& path = "data/users.csv");

    // ── Backup / Restore ──────────────────────────────────────────
    static void backup();
    static void restore();
    static void clearAllTasks();
    static void clearAllUsers();

    // ── Utility ───────────────────────────────────────────────────
    static void ensureDataDir();

private:
    static string readFile(const string& path);
    static void   writeFile(const string& path, const string& content);
    static bool   copyFile(const string& src, const string& dst);
};