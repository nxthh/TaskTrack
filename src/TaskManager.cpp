#include "TaskManager.h"
#include "FileManager.h"
#include <algorithm>
#include <iostream>
#include <tabulate/table.hpp>

using namespace std;
using namespace tabulate;

// ── helpers ──────────────────────────────────────────────────────────────────

void TaskManager::recomputeNextId() {
  nextId = 1;
  for (auto &t : tasks)
    if (t.getId() >= nextId)
      nextId = t.getId() + 1;
  for (auto &t : trash)
    if (t.getId() >= nextId)
      nextId = t.getId() + 1;
}

static string priorityBadge(const string &p) {
  if (p == "High")
    return "[H] High";
  if (p == "Medium")
    return "[M] Medium";
  return "[L] Low";
}

// Returns the tabulate Color for a given status string
static Color statusColor(const string &s) {
  if (s == "Done")
    return Color::green;
  if (s == "In-Progress")
    return Color::yellow;
  return Color::white; // To-Do
}

// Prints a status label to stdout with ANSI colour, then resets
static void printColoredStatus(const string &s) {
  // ANSI codes: yellow = 33, green = 32, white/reset = 0
  if (s == "Done")
    cout << "\033[32m" << s << "\033[0m";
  else if (s == "In-Progress")
    cout << "\033[33m" << s << "\033[0m";
  else
    cout << s; // To-Do — no colour
}

void TaskManager::printTable(const vector<Task> &list) const {
  if (list.empty()) {
    cout << "\n  (no tasks to display)\n";
    return;
  }

  Table t;
  t.add_row(
      {"ID", "Title", "Priority", "Status", "Due Date", "Owner", "Created"});
  t[0].format().font_style({FontStyle::bold}).font_color(Color::cyan);

  for (auto &task : list) {
    string due = task.getDueDate();
    if (task.isOverdue())
      due += " [OVERDUE]";

    Color rowColour = Color::white;
    if (task.getStatus() == "Done")
      rowColour = Color::green;
    else if (task.isOverdue())
      rowColour = Color::red;
    else if (task.getStatus() == "In-Progress")
      rowColour = Color::yellow;

    t.add_row({to_string(task.getId()), task.getTitle(),
               priorityBadge(task.getPriority()), task.getStatus(), due,
               task.getAssignee(), task.getCreatedDate()});

    size_t row = t.size() - 1;
    t[row].format().font_color(rowColour);
  }

  cout << "\n" << t << "\n";
}

static vector<Task> filterOwner(const vector<Task> &src, const string &owner,
                                bool isAdmin) {
  if (isAdmin)
    return src;
  vector<Task> out;
  for (auto &t : src)
    if (t.getAssignee() == owner)
      out.push_back(t);
  return out;
}

// ── Lifecycle
// ─────────────────────────────────────────────────────────────────

void TaskManager::loadFromFile() {
  tasks = FileManager::loadTasks();
  recomputeNextId();
}

void TaskManager::saveToFile() const { FileManager::saveTasks(tasks); }

void TaskManager::loadTrash() { trash = FileManager::loadTrash(); }

void TaskManager::saveTrash() const { FileManager::saveTrash(trash); }

// ── CRUD ─────────────────────────────────────────────────────────────────────

void TaskManager::addTask(const string &ownerUsername) {
  Task t;
  t.input(ownerUsername, nextId++);
  tasks.push_back(t);
  saveToFile();
  cout << "  Task added (ID: " << t.getId() << ").\n";
}

void TaskManager::editTask(int id, const string &owner, bool isAdmin) {
  for (auto &t : tasks) {
    if (t.getId() == id) {
      if (!isAdmin && t.getAssignee() != owner) {
        cout << "  Permission denied.\n";
        return;
      }
      t.edit();
      saveToFile();
      cout << "  Task updated.\n";
      return;
    }
  }
  cout << "  Task not found.\n";
}

void TaskManager::deleteTask(int id, const string &owner, bool isAdmin) {
  auto it = find_if(tasks.begin(), tasks.end(),
                    [&](Task &t) { return t.getId() == id; });
  if (it == tasks.end()) {
    cout << "  Task not found.\n";
    return;
  }
  if (!isAdmin && it->getAssignee() != owner) {
    cout << "  Permission denied.\n";
    return;
  }
  it->setDeleted(true);
  trash.push_back(*it);
  tasks.erase(it);
  saveToFile();
  saveTrash();
  cout << "  Task moved to Recovery.\n";
}

void TaskManager::showAll(const string &owner, bool isAdmin) const {
  auto list = filterOwner(tasks, owner, isAdmin);
  cout << "\n=== All Tasks ===";
  printTable(list);
}

// ── Status pipeline
// ───────────────────────────────────────────────────────────

void TaskManager::advanceStatus(int id, const string &owner, bool isAdmin) {
  for (auto &t : tasks) {
    if (t.getId() == id) {
      if (!isAdmin && t.getAssignee() != owner) {
        cout << "  Permission denied.\n";
        return;
      }

      string before = t.getStatus();
      t.advanceStatus();
      string after = t.getStatus();

      if (after != before) {
        cout << "  Status: ";
        printColoredStatus(before);
        cout << " -> ";
        printColoredStatus(after);
        cout << "\n";
      }

      saveToFile();
      return;
    }
  }
  cout << "  Task not found.\n";
}

// ── Dashboard counts
// ──────────────────────────────────────────────────────────

int TaskManager::totalTasks() const { return (int)tasks.size(); }

int TaskManager::completedTasks(const string &owner, bool isAdmin) const {
  auto list = filterOwner(tasks, owner, isAdmin);
  int cnt = 0;
  for (auto &t : list)
    if (t.getStatus() == "Done")
      cnt++;
  return cnt;
}

int TaskManager::pendingTasks(const string &owner, bool isAdmin) const {
  auto list = filterOwner(tasks, owner, isAdmin);
  int cnt = 0;
  for (auto &t : list)
    if (t.getStatus() != "Done")
      cnt++;
  return cnt;
}

// ── Dashboard table with colored counts
// ──────────────────────────────────────

void TaskManager::printDashboardTable(const string &owner, bool isAdmin,
                                      int completed, int pending,
                                      int total) const {
  Table t;
  t.add_row({"Metric", "Count"});
  t[0].format().font_style({FontStyle::bold}).font_color(Color::cyan);

  if (isAdmin) {
    t.add_row({"Total Tasks (system-wide)", to_string(totalTasks())});
    t.add_row({"Completed Tasks (yours)", to_string(completed)});
    t.add_row({"Pending Tasks (yours)", to_string(pending)});

    // colour the completed row green (row index 2)
    t[2][1].format().font_color(Color::green);
  } else {
    t.add_row({"Completed Tasks", to_string(completed)});
    t.add_row({"Pending Tasks", to_string(pending)});
    t.add_row({"Total Tasks", to_string(total)});

    // colour the completed row green (row index 1)
    t[1][1].format().font_color(Color::green);
  }

  t.format()
      .border_top("-")
      .border_bottom("-")
      .border_left("|")
      .border_right("|")
      .corner("+");

  cout << t << "\n";
}

// ── Trash / Recovery
// ──────────────────────────────────────────────────────────

void TaskManager::showTrash() const {
  cout << "\n=== Recovery (Trash) ===";
  printTable(trash);
}

void TaskManager::restoreTask(int id) {
  auto it = find_if(trash.begin(), trash.end(),
                    [&](Task &t) { return t.getId() == id; });
  if (it == trash.end()) {
    cout << "  Task not found in Recovery.\n";
    return;
  }
  it->setDeleted(false);
  tasks.push_back(*it);
  trash.erase(it);
  saveToFile();
  saveTrash();
  cout << "  Task restored successfully.\n";
}

void TaskManager::permanentDelete(int id) {
  auto it = find_if(trash.begin(), trash.end(),
                    [&](Task &t) { return t.getId() == id; });
  if (it == trash.end()) {
    cout << "  Task not found in Recovery.\n";
    return;
  }
  cout << "  Permanently delete task \"" << it->getTitle() << "\"? (y/n): ";
  char c;
  cin >> c;
  if (c == 'y' || c == 'Y') {
    trash.erase(it);
    saveTrash();
    cout << "  Task permanently deleted.\n";
  }
}

void TaskManager::emptyTrash() {
  if (trash.empty()) {
    cout << "  Recovery is already empty.\n";
    return;
  }
  cout << "  Permanently delete ALL " << trash.size()
       << " items in Recovery? (y/n): ";
  char c;
  cin >> c;
  if (c == 'y' || c == 'Y') {
    trash.clear();
    saveTrash();
    cout << "  Recovery emptied.\n";
  }
}

// ── Admin clear
// ───────────────────────────────────────────────────────────────

void TaskManager::clearAllTasks() {
  cout << "  Clear ALL tasks and Recovery? (y/n): ";
  char c;
  cin >> c;
  if (c == 'y' || c == 'Y') {
    tasks.clear();
    trash.clear();
    nextId = 1;
    FileManager::clearAllTasks();
    cout << "  All tasks and Recovery cleared.\n";
  }
}
