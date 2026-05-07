#include "TaskManager.h"
#include "FileManager.h"
#include <iostream>
#include <algorithm>
#include <tabulate/table.hpp>

using namespace std;
using namespace tabulate;

// ── helpers ──────────────────────────────────────────────────────────────────

void TaskManager::recomputeNextId() {
    nextId = 1;
    for (auto& t : tasks)
        if (t.getId() >= nextId) nextId = t.getId() + 1;
    for (auto& t : trash)
        if (t.getId() >= nextId) nextId = t.getId() + 1;
}

static string priorityBadge(const string& p) {
    if (p == "High")   return "[H] " + p;
    if (p == "Medium") return "[M] " + p;
    return "[L] " + p;
}

void TaskManager::printTable(const vector<Task>& list) const {
    if (list.empty()) { cout << "  No tasks to show.\n"; return; }

    Table t;
    t.add_row({"ID","Title","Status","Priority","Due Date","Category","Owner","Created"});
    t[0].format().font_style({FontStyle::bold});

    for (auto& task : list) {
        string due = task.getDueDate();
        if (task.isOverdue()) due += " !!";
        t.add_row({
            to_string(task.getId()),
            task.getTitle(),
            task.getStatus(),
            priorityBadge(task.getPriority()),
            due,
            task.getCategory(),
            task.getAssignee(),
            task.getCreatedDate()
        });
    }
    cout << "\n" << t << "\n";
}

static vector<Task> filterOwner(const vector<Task>& src,
                                const string& owner, bool isAdmin) {
    if (isAdmin) return src;
    vector<Task> out;
    for (auto& t : src)
        if (t.getAssignee() == owner) out.push_back(t);
    return out;
}

// ── Lifecycle ─────────────────────────────────────────────────────────────────

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

void TaskManager::addTask(const string& ownerUsername) {
    Task t;
    t.input(ownerUsername, nextId++);
    tasks.push_back(t);
    saveToFile();
    cout << "  Task added (ID: " << t.getId() << ").\n";
}

void TaskManager::editTask(int id, const string& owner, bool isAdmin) {
    for (auto& t : tasks) {
        if (t.getId() == id) {
            if (!isAdmin && t.getAssignee() != owner) {
                cout << "  Permission denied.\n"; return;
            }
            t.edit();
            saveToFile();
            cout << "  Task updated.\n";
            return;
        }
    }
    cout << "  Task not found.\n";
}

void TaskManager::deleteTask(int id, const string& owner, bool isAdmin) {
    auto it = find_if(tasks.begin(), tasks.end(),
        [&](Task& t){ return t.getId() == id; });
    if (it == tasks.end()) { cout << "  Task not found.\n"; return; }
    if (!isAdmin && it->getAssignee() != owner) {
        cout << "  Permission denied.\n"; return;
    }
    it->setDeleted(true);
    trash.push_back(*it);
    tasks.erase(it);
    saveToFile();
    saveTrash();
    cout << "  Task moved to trash (use 'Restore' to recover).\n";
}

void TaskManager::showAll(const string& owner, bool isAdmin) const {
    auto list = filterOwner(tasks, owner, isAdmin);
    printTable(list);
}

// ── Status pipeline ───────────────────────────────────────────────────────────

void TaskManager::advanceStatus(int id, const string& owner, bool isAdmin) {
    for (auto& t : tasks) {
        if (t.getId() == id) {
            if (!isAdmin && t.getAssignee() != owner) {
                cout << "  Permission denied.\n"; return;
            }
            string before = t.getStatus();
            t.advanceStatus();
            cout << "  Status: " << before << " → " << t.getStatus() << "\n";
            saveToFile();
            return;
        }
    }
    cout << "  Task not found.\n";
}

// ── Smart views ───────────────────────────────────────────────────────────────

void TaskManager::showToday(const string& owner, bool isAdmin) const {
    vector<Task> out;
    for (auto& t : filterOwner(tasks, owner, isAdmin))
        if (t.isDueToday()) out.push_back(t);
    cout << "\n=== Today's Tasks ===\n";
    printTable(out);
}

void TaskManager::showUpcoming(const string& owner, bool isAdmin) const {
    vector<Task> out;
    for (auto& t : filterOwner(tasks, owner, isAdmin))
        if (t.isUpcoming()) out.push_back(t);
    cout << "\n=== Upcoming Tasks ===\n";
    printTable(out);
}

void TaskManager::showOverdue(const string& owner, bool isAdmin) const {
    vector<Task> out;
    for (auto& t : filterOwner(tasks, owner, isAdmin))
        if (t.isOverdue()) out.push_back(t);
    cout << "\n=== Overdue Tasks ===\n";
    printTable(out);
}

// ── Search & Filter ───────────────────────────────────────────────────────────

void TaskManager::search(const string& owner, bool isAdmin) const {
    string kw;
    cout << "Search keyword: ";
    cin.ignore();
    getline(cin, kw);

    auto base = filterOwner(tasks, owner, isAdmin);
    vector<Task> out;
    for (auto& t : base) {
        if (t.getTitle().find(kw)       != string::npos ||
            t.getDescription().find(kw) != string::npos ||
            t.getCategory().find(kw)    != string::npos)
            out.push_back(t);
    }
    printTable(out);
}

void TaskManager::filterByStatus(const string& owner, bool isAdmin) const {
    string val;
    cout << "Status (To-Do / In-Progress / Done): ";
    cin.ignore(); getline(cin, val);

    auto base = filterOwner(tasks, owner, isAdmin);
    vector<Task> out;
    for (auto& t : base)
        if (t.getStatus() == val) out.push_back(t);
    printTable(out);
}

void TaskManager::filterByPriority(const string& owner, bool isAdmin) const {
    string val;
    cout << "Priority (High / Medium / Low): ";
    cin.ignore(); getline(cin, val);

    auto base = filterOwner(tasks, owner, isAdmin);
    vector<Task> out;
    for (auto& t : base)
        if (t.getPriority() == val) out.push_back(t);
    printTable(out);
}

void TaskManager::filterByDate(const string& owner, bool isAdmin) const {
    cout << "Date filter:\n";
    cout << "  1. Today\n  2. Upcoming\n  3. Overdue\n";
    cout << "Choice: ";
    int ch; cin >> ch;
    if (ch == 1) showToday(owner, isAdmin);
    else if (ch == 2) showUpcoming(owner, isAdmin);
    else if (ch == 3) showOverdue(owner, isAdmin);
    else cout << "  Invalid choice.\n";
}

// ── Sort ─────────────────────────────────────────────────────────────────────

void TaskManager::sortTasks(const string& owner, bool isAdmin) {
    cout << "Sort by (id / due / priority / status): ";
    string type; cin >> type;

    auto cmpPriority = [](const Task& a, const Task& b) {
        // High > Medium > Low
        auto rank = [](const string& p) {
            if (p == "High")   return 0;
            if (p == "Medium") return 1;
            return 2;
        };
        return rank(a.getPriority()) < rank(b.getPriority());
    };

    if      (type == "id")       sort(tasks.begin(), tasks.end(),
        [](Task& a, Task& b){ return a.getId() < b.getId(); });
    else if (type == "due")      sort(tasks.begin(), tasks.end(),
        [](Task& a, Task& b){ return a.getDueDate() < b.getDueDate(); });
    else if (type == "priority") sort(tasks.begin(), tasks.end(), cmpPriority);
    else if (type == "status")   sort(tasks.begin(), tasks.end(),
        [](Task& a, Task& b){ return a.getStatus() < b.getStatus(); });
    else { cout << "  Unknown sort key.\n"; return; }

    saveToFile();
    cout << "  Tasks sorted by " << type << ".\n";
}

// ── Trash / Recovery ──────────────────────────────────────────────────────────

void TaskManager::showTrash() const {
    cout << "\n=== Trash (soft-deleted tasks) ===\n";
    printTable(trash);
}

void TaskManager::restoreTask(int id) {
    auto it = find_if(trash.begin(), trash.end(),
        [&](Task& t){ return t.getId() == id; });
    if (it == trash.end()) { cout << "  Task not in trash.\n"; return; }
    it->setDeleted(false);
    tasks.push_back(*it);
    trash.erase(it);
    saveToFile();
    saveTrash();
    cout << "  Task restored.\n";
}

void TaskManager::emptyTrash() {
    cout << "  Empty trash? This is permanent. (y/n): ";
    char c; cin >> c;
    if (c == 'y' || c == 'Y') {
        trash.clear();
        saveTrash();
        cout << "  Trash emptied.\n";
    }
}

// ── Admin clear ───────────────────────────────────────────────────────────────

void TaskManager::clearAllTasks() {
    cout << "  Clear ALL tasks? (y/n): ";
    char c; cin >> c;
    if (c == 'y' || c == 'Y') {
        tasks.clear();
        trash.clear();
        nextId = 1;
        FileManager::clearAllTasks();
        cout << "  All tasks and trash cleared.\n";
    }
}

// ── Statistics ────────────────────────────────────────────────────────────────

void TaskManager::statistics(const string& owner, bool isAdmin) const {
    auto list = filterOwner(tasks, owner, isAdmin);
    int total = (int)list.size();
    int todo = 0, prog = 0, done = 0, overdue = 0;

    for (auto& t : list) {
        if (t.getStatus() == "To-Do")       todo++;
        else if (t.getStatus() == "In-Progress") prog++;
        else if (t.getStatus() == "Done")   done++;
        if (t.isOverdue())                  overdue++;
    }

    int width = 30;
    int filled = total ? (done * width / total) : 0;

    cout << "\n========== STATISTICS ==========\n";
    cout << "Total Tasks   : " << total   << "\n";
    cout << "To-Do         : " << todo    << "\n";
    cout << "In-Progress   : " << prog    << "\n";
    cout << "Done          : " << done    << "\n";
    cout << "Overdue       : " << overdue << "\n";
    cout << "Progress: [";
    for (int i = 0; i < width; i++) cout << (i < filled ? "#" : "-");
    cout << "] " << (total ? (done * 100 / total) : 0) << "% complete\n";
    cout << "================================\n";
}
