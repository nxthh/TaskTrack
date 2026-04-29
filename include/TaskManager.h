#pragma once
#include <vector>
#include "Task.h"

class TaskManager {
private:
    vector<Task> tasks;
    vector<Task> trash;
    int          nextId = 1;

    void   recomputeNextId();
    void   printTable(const vector<Task>& list) const;

public:
    // ── Lifecycle ─────────────────────────────────────────────
    void loadFromFile();
    void saveToFile()  const;
    void loadTrash();
    void saveTrash()   const;

    // ── CRUD ──────────────────────────────────────────────────
    void addTask(const string& ownerUsername);
    void editTask(int id, const string& ownerUsername, bool isAdmin);
    void deleteTask(int id, const string& ownerUsername, bool isAdmin);
    void showAll(const string& ownerUsername, bool isAdmin) const;

    // ── Status pipeline ───────────────────────────────────────
    void advanceStatus(int id, const string& ownerUsername, bool isAdmin);

    // ── Smart views ───────────────────────────────────────────
    void showToday(const string& ownerUsername, bool isAdmin)    const;
    void showUpcoming(const string& ownerUsername, bool isAdmin) const;
    void showOverdue(const string& ownerUsername, bool isAdmin)  const;

    // ── Search & Filter ───────────────────────────────────────
    void search(const string& ownerUsername, bool isAdmin)        const;
    void filterByStatus(const string& ownerUsername, bool isAdmin) const;
    void filterByPriority(const string& ownerUsername, bool isAdmin) const;
    void filterByDate(const string& ownerUsername, bool isAdmin)   const;

    // ── Sort ──────────────────────────────────────────────────
    void sortTasks(const string& ownerUsername, bool isAdmin);

    // ── Trash / Recovery ──────────────────────────────────────
    void showTrash() const;
    void restoreTask(int id);
    void emptyTrash();

    // ── Admin clear ───────────────────────────────────────────
    void clearAllTasks();

    // ── Statistics ────────────────────────────────────────────
    void statistics(const string& ownerUsername, bool isAdmin) const;
};
