#pragma once
#include "Task.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

class TaskManager {
private:
    vector<Task> tasks;
    vector<Task> trash;
    int nextId = 1;

    void recomputeNextId();
    int nextIdForUser(const string &owner) const; 
    void printTable(const vector<Task> &list) const;

public:
    // ── Lifecycle ─────────────────────────────────────────────
    void loadFromFile();
    void saveToFile() const;
    void loadTrash();
    void saveTrash() const;

    // ── Task Operations ───────────────────────────────────────
    void addTask(const string &ownerUsername);
    void editTask(int id, const string &ownerUsername, bool isAdmin);
    void deleteTask(int id, const string &ownerUsername, bool isAdmin);
    void showAll(const string &ownerUsername, bool isAdmin) const;
    void advanceStatus(int id, const string &ownerUsername, bool isAdmin);

    // ── Search & Sort ─────────────────────────────────────────
    void searchTasks(const string &keyword, const string &owner, bool isAdmin) const;
    void filterByStatus(const string &status, const string &owner, bool isAdmin) const;
    void sortByDeadline(const string &owner, bool isAdmin);
    void sortByPriority(const string &owner, bool isAdmin);

    // ── Dashboard Methods ────────────────────────────────────
    int totalTasks() const;
    int completedTasks(const string &owner, bool isAdmin) const;
    int pendingTasks(const string &owner, bool isAdmin) const;
    
    // These specific lines resolve the red squiggles in System.cpp
    int totalCompletedSystemWide() const; 
    int totalPendingSystemWide() const;   

    // ── Trash & Admin ─────────────────────────────────────────
    void showTrash(const string &ownerUsername, bool isAdmin) const; 
    void restoreTask(int id, const string &ownerUsername, bool isAdmin);
    void permanentDelete(int id, const string &ownerUsername, bool isAdmin);
    void emptyTrash();
    void clearAllTasks();
};