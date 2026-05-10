#include "TaskManager.h"
#include "FileManager.h"
#include <algorithm>
#include <iostream>
#include <tabulate/table.hpp>

using namespace std;
using namespace tabulate;

// ── Helpers ──────────────────────────────────────────────────────────────────

int TaskManager::nextIdForUser(const string &owner) const {
    int id = 1;
    for (auto &t : tasks)
        if (t.getAssignee() == owner && t.getId() >= id)
            id = t.getId() + 1;
    for (auto &t : trash)
        if (t.getAssignee() == owner && t.getId() >= id)
            id = t.getId() + 1;
    return id;
}

void TaskManager::recomputeNextId() {
    nextId = 1; // Start from 1
    for (auto &t : tasks) {
        if (t.getId() >= nextId) {
            nextId = t.getId() + 1; // Use nextId, not id
        }
    }
    for (auto &t : trash) {
        if (t.getId() >= nextId) {
            nextId = t.getId() + 1; // Use nextId, not id
        }
    }
}

static string priorityBadge(const string &p) {
    if (p == "High") return "[H] High";
    if (p == "Medium") return "[M] Medium";
    return "[L] Low";
}

static void printColoredStatus(const string &s) {
    if (s == "Done") cout << "\033[32m" << s << "\033[0m";
    else if (s == "In-Progress") cout << "\033[33m" << s << "\033[0m";
    else cout << s;
}

void TaskManager::printTable(const vector<Task> &list) const {
    if (list.empty()) {
        cout << "\n   (no tasks to display)\n";
        return;
    }

    Table t;
    t.add_row({"ID", "Title", "Priority", "Status", "Due Date", "Owner", "Created"});
    t[0].format().font_style({FontStyle::bold}).font_color(Color::cyan);

    for (auto &task : list) {
        string due = task.getDueDate();
        if (task.isOverdue()) due += " [OVERDUE]";

        Color rowColour = Color::white;
        if (task.getStatus() == "Done") rowColour = Color::green;
        else if (task.isOverdue()) rowColour = Color::red;
        else if (task.getStatus() == "In-Progress") rowColour = Color::yellow;

        t.add_row({to_string(task.getId()), task.getTitle(),
                   priorityBadge(task.getPriority()), task.getStatus(), due,
                   task.getAssignee(), task.getCreatedDate()});

        size_t row = t.size() - 1;
        t[row].format().font_color(rowColour);
    }
    cout << "\n" << t << "\n";
}

static vector<Task> filterOwner(const vector<Task> &src, const string &owner, bool isAdmin) {
    if (isAdmin) return src;
    vector<Task> out;
    for (auto &t : src)
        if (t.getAssignee() == owner)
            out.push_back(t);
    return out;
}

// ── Lifecycle ────────────────────────────────────────────────────────────────

void TaskManager::loadFromFile() {
    tasks = FileManager::loadTasks();
    recomputeNextId();
}

void TaskManager::saveToFile() const { 
    FileManager::saveTasks(tasks); 
}

void TaskManager::loadTrash() { 
    trash = FileManager::loadTrash(); 
}

void TaskManager::saveTrash() const { 
    FileManager::saveTrash(trash); 
}

// ── CRUD ─────────────────────────────────────────────────────────────────────

void TaskManager::addTask(const string &ownerUsername) {
    Task t;
    t.input(ownerUsername, nextIdForUser(ownerUsername));
    tasks.push_back(t);
    saveToFile();
    cout << "   Task added successfully (ID: " << t.getId() << ").\n";
}

void TaskManager::editTask(int id, const string &owner, bool isAdmin) {
    bool found = false;
    for (auto &t : tasks) {
        if (t.getId() == id && (isAdmin || t.getAssignee() == owner)) {
            t.edit();
            saveToFile();
            cout << "   Task updated successfully.\n";
            found = true;
            break;
        }
    }
    if (!found) cout << "   Task ID " << id << " not found or access denied.\n";
}

void TaskManager::deleteTask(int id, const string &owner, bool isAdmin) {
    auto it = find_if(tasks.begin(), tasks.end(), [&](const Task &t) {
        return t.getId() == id && (isAdmin || t.getAssignee() == owner);
    });
    
    if (it == tasks.end()) {
        cout << "   Task not found.\n";
        return;
    }

    trash.push_back(*it);
    tasks.erase(it);
    saveToFile();
    saveTrash();
    cout << "   Task moved to Recovery (Trash).\n";
}

void TaskManager::showAll(const string &owner, bool isAdmin) const {
    auto list = filterOwner(tasks, owner, isAdmin);
    cout << "\n   === ACTIVE TASK LIST (" << (isAdmin ? "ADMIN VIEW" : owner) << ") ===";
    printTable(list);
}

// ── Status Pipeline ──────────────────────────────────────────────────────────

void TaskManager::advanceStatus(int id, const string &owner, bool isAdmin) {
    for (auto &t : tasks) {
        if (t.getId() == id && (isAdmin || t.getAssignee() == owner)) {
            string before = t.getStatus();
            t.advanceStatus();
            string after = t.getStatus();
            
            if (after != before) {
                cout << "   Status updated: "; 
                printColoredStatus(before);
                cout << " -> "; 
                printColoredStatus(after); 
                cout << "\n";
            }
            saveToFile();
            return;
        }
    }
    cout << "   Task not found.\n";
}

// ── Search, Sort, Filter ─────────────────────────────────────────────────────

void TaskManager::searchTasks(const string &keyword, const string &owner, bool isAdmin) const {
    auto list = filterOwner(tasks, owner, isAdmin);
    vector<Task> results;
    for (const auto& t : list) {
        if (t.getTitle().find(keyword) != string::npos || t.getDescription().find(keyword) != string::npos) {
            results.push_back(t);
        }
    }
    cout << "\n   === SEARCH RESULTS FOR: '" << keyword << "' ===";
    printTable(results);
}

void TaskManager::filterByStatus(const string &status, const string &owner, bool isAdmin) const {
    auto list = filterOwner(tasks, owner, isAdmin);
    vector<Task> filtered;
    for (const auto& t : list) {
        if (t.getStatus() == status) {
            filtered.push_back(t);
        }
    }
    cout << "\n   === FILTERED BY STATUS: " << status << " ===";
    printTable(filtered);
}

void TaskManager::sortByDeadline(const string &owner, bool isAdmin) {
    sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.getDueDate() < b.getDueDate();
    });
    cout << "   [System] Sorted by Deadline (Ascending)";
    showAll(owner, isAdmin);
}

void TaskManager::sortByPriority(const string &owner, bool isAdmin) {
    auto pVal = [](const string& p) {
        if (p == "High") return 3;
        if (p == "Medium") return 2;
        return 1;
    };
    sort(tasks.begin(), tasks.end(), [&](const Task& a, const Task& b) {
        return pVal(a.getPriority()) > pVal(b.getPriority());
    });
    cout << "   [System] Sorted by Priority (High to Low)";
    showAll(owner, isAdmin);
}

// ── Dashboard Counts ─────────────────────────────────────────────────────────

int TaskManager::totalTasks() const { 
    return (int)tasks.size(); 
}

int TaskManager::completedTasks(const string &owner, bool isAdmin) const {
    auto list = filterOwner(tasks, owner, isAdmin);
    int cnt = 0;
    for (auto &t : list) if (t.getStatus() == "Done") cnt++;
    return cnt;
}

int TaskManager::pendingTasks(const string &owner, bool isAdmin) const {
    auto list = filterOwner(tasks, owner, isAdmin);
    int cnt = 0;
    for (auto &t : list) if (t.getStatus() != "Done") cnt++;
    return cnt;
}

// --- ADDED THESE TO FIX SYSTEM.CPP ERRORS ---
int TaskManager::totalCompletedSystemWide() const {
    int cnt = 0;
    for (const auto &t : tasks) if (t.getStatus() == "Done") cnt++;
    return cnt;
}

int TaskManager::totalPendingSystemWide() const {
    int cnt = 0;
    for (const auto &t : tasks) if (t.getStatus() != "Done") cnt++;
    return cnt;
}

// ── Trash / Recovery ─────────────────────────────────────────────────────────

void TaskManager::showTrash(const string &owner, bool isAdmin) const {
    auto list = filterOwner(trash, owner, isAdmin);
    cout << "\n   === RECOVERY HUB (TRASH) ===";
    printTable(list);
}

void TaskManager::restoreTask(int id, const string &owner, bool isAdmin) {
    auto it = find_if(trash.begin(), trash.end(), [&](const Task &t) {
        return t.getId() == id && (isAdmin || t.getAssignee() == owner);
    });
    
    if (it == trash.end()) {
        cout << "   Task not found in Trash.\n";
        return;
    }

    tasks.push_back(*it);
    trash.erase(it);
    saveToFile();
    saveTrash();
    cout << "   Success: Task ID " << id << " restored to active list.\n";
}

void TaskManager::permanentDelete(int id, const string &owner, bool isAdmin) {
    auto it = find_if(trash.begin(), trash.end(), [&](const Task &t) {
        return t.getId() == id && (isAdmin || t.getAssignee() == owner);
    });
    
    if (it == trash.end()) {
        cout << "   Task not found in Trash.\n";
        return;
    }

    cout << "   WARNING: Permanent deletion cannot be undone.\n";
    cout << "   Delete \"" << it->getTitle() << "\"? (y/n): ";
    char c; cin >> c;
    if (c == 'y' || c == 'Y') {
        trash.erase(it);
        saveTrash();
        cout << "   Task permanently wiped from storage.\n";
    }
}

void TaskManager::clearAllTasks() {
    cout << "   CRITICAL: This will wipe ALL tasks system-wide.\n";
    cout << "   Confirm complete system wipe? (y/n): ";
    char c; cin >> c;
    if (c == 'y' || c == 'Y') {
        tasks.clear();
        trash.clear();
        nextId = 1;
        FileManager::clearAllTasks();
        cout << "   All data cleared. System reset.\n";
    }
}