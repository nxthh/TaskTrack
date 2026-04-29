#pragma once
#include <vector>
#include <string>
#include "Task.h"
#include "User.h"

class FileManager {
public:
    // ── Tasks ─────────────────────────────────────────────────
    static void saveTasks(const vector<Task>& tasks,
                          const string& path = "data/tasks.json");
    static vector<Task> loadTasks(const string& path = "data/tasks.json");

    // ── Trash (soft-deleted tasks) ────────────────────────────
    static void saveTrash(const vector<Task>& trash,
                          const string& path = "data/trash.json");
    static vector<Task> loadTrash(const string& path = "data/trash.json");

    // ── Users ─────────────────────────────────────────────────
    static void saveUsers(const vector<User>& users,
                          const string& path = "data/users.json");
    static vector<User> loadUsers(const string& path = "data/users.json");

    // ── Backup / Restore ──────────────────────────────────────
    static void backup();          // copies data/ → data/backup/
    static void restore();         // copies data/backup/ → data/
    static void clearAllTasks();   // truncates tasks.json + trash.json
    static void clearAllUsers();   // truncates users.json (keeps first admin)

    // ── Utility ───────────────────────────────────────────────
    static void ensureDataDir();
private:
    static string readFile(const string& path);
    static void   writeFile(const string& path, const string& content);
    static void   copyFile(const string& src, const string& dst);
};
