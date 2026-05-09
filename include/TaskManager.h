#pragma once
#include "Task.h"
#include <vector>
using std::string;
using std::vector;

class TaskManager {
private:
  vector<Task> tasks;
  vector<Task> trash;
  int nextId = 1;

  void recomputeNextId();
  int nextIdForUser(const string &owner) const; // per-user ID counter
  void printTable(const vector<Task> &list) const;

public:
  // ── Lifecycle ─────────────────────────────────────────────
  void loadFromFile();
  void saveToFile() const;
  void loadTrash();
  void saveTrash() const;

  // ── CRUD ──────────────────────────────────────────────────
  void addTask(const string &ownerUsername);
  void editTask(int id, const string &ownerUsername, bool isAdmin);
  void deleteTask(int id, const string &ownerUsername, bool isAdmin);
  void showAll(const string &ownerUsername, bool isAdmin) const;

  // ── Status pipeline ───────────────────────────────────────
  void advanceStatus(int id, const string &ownerUsername, bool isAdmin);

  // ── Dashboard counts ──────────────────────────────────────
  int totalTasks() const;
  int completedTasks(const string &ownerUsername, bool isAdmin) const;
  int pendingTasks(const string &ownerUsername, bool isAdmin) const;

  // ── Dashboard table (coloured completed count) ────────────
  void printDashboardTable(const string &ownerUsername, bool isAdmin,
                           int completed, int pending, int total) const;

  // ── Trash / Recovery ──────────────────────────────────────
  void showTrash() const;
  void showTrashFor(const string &ownerUsername, bool isAdmin) const;
  void restoreTask(int id, const string &ownerUsername, bool isAdmin);
  void permanentDelete(int id, const string &ownerUsername, bool isAdmin);
  void emptyTrash();

  // ── Admin clear ───────────────────────────────────────────
  void clearAllTasks();
};