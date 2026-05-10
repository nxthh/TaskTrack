#include "FileManager.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;
using namespace std;

// ── Low-Level File I/O ──────────────────────────────────────────────────────

string FileManager::readFile(const string &path) {
    ifstream f(path);
    if (!f.is_open()) return "";
    ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

void FileManager::writeFile(const string &path, const string &content) {
    ensureDataDir(); // Safety: make sure folder exists before writing
    ofstream f(path);
    if (!f.is_open()) {
        cerr << "  [FileManager] Cannot write to: " << path << "\n";
        return;
    }
    f << content;
}

bool FileManager::copyFile(const string &src, const string &dst) {
    try {
        if (!fs::exists(src)) return false;
        // Use copy_file with overwrite to ensure backup/restore works every time
        fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
        return true;
    } catch (...) {
        return false;
    }
}

void FileManager::ensureDataDir() {
    if (!fs::exists("data")) fs::create_directories("data");
    if (!fs::exists("data/backup")) fs::create_directories("data/backup");
}

// ── CSV Helpers (RFC-4180 Compliant) ────────────────────────────────────────

static string csvEscape(const string &field) {
    bool needsQuote = (field.find(',') != string::npos || field.find('"') != string::npos ||
                       field.find('\n') != string::npos || field.find('\r') != string::npos);
    if (!needsQuote) return field;
    string out = "\"";
    for (char c : field) {
        if (c == '"') out += "\"\"";
        else out += c;
    }
    out += '"';
    return out;
}

static vector<string> csvParseLine(const string &line) {
    vector<string> fields;
    string field;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') { field += '"'; i++; }
                else inQuotes = false;
            } else field += c;
        } else {
            if (c == '"') inQuotes = true;
            else if (c == ',') { fields.push_back(field); field.clear(); }
            else if (c == '\r') continue;
            else field += c;
        }
    }
    fields.push_back(field);
    return fields;
}

static vector<string> splitLines(const string &text) {
    vector<string> lines;
    istringstream ss(text);
    string line;
    while (getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

// ── Tasks ────────────────────────────────────────────────────────────────────

static const string TASK_HEADER = "id,title,description,priority,status,dueDate,createdDate,assignee,deleted";

static string encodeTask(const Task &t) {
    return csvEscape(to_string(t.getId())) + "," + csvEscape(t.getTitle()) + "," +
           csvEscape(t.getDescription()) + "," + csvEscape(t.getPriority()) + "," +
           csvEscape(t.getStatus()) + "," + csvEscape(t.getDueDate()) + "," +
           csvEscape(t.getCreatedDate()) + "," + csvEscape(t.getAssignee()) + "," +
           csvEscape(t.isDeleted() ? "true" : "false");
}

static Task decodeTask(const vector<string> &f) {
    if (f.size() < 9) return Task{};
    Task t;
    try {
        t.setId(f[0].empty() ? 0 : stoi(f[0]));
    } catch (...) { t.setId(0); }
    
    t.setTitle(f[1]);
    t.setDescription(f[2]);
    t.setPriority(f[3]);
    t.setStatus(f[4]);
    t.setDueDate(f[5]);
    t.setCreatedDate(f[6]);
    t.setAssignee(f[7]);
    t.setDeleted(f[8] == "true");
    return t;
}

void FileManager::saveTasks(const vector<Task> &tasks, const string &path) {
    string out = TASK_HEADER + "\n";
    for (const auto &t : tasks) out += encodeTask(t) + "\n";
    writeFile(path, out);
}

vector<Task> FileManager::loadTasks(const string &path) {
    vector<Task> result;
    string raw = readFile(path);
    if (raw.empty()) return result;
    auto lines = splitLines(raw);
    for (size_t i = 1; i < lines.size(); i++) 
        result.push_back(decodeTask(csvParseLine(lines[i])));
    return result;
}

// ── Trash ────────────────────────────────────────────────────────────────────

void FileManager::saveTrash(const vector<Task> &trash, const string &path) {
    saveTasks(trash, path);
}

vector<Task> FileManager::loadTrash(const string &path) {
    return loadTasks(path);
}

// ── Users ────────────────────────────────────────────────────────────────────

static const string USER_HEADER = "username,password,fullName,gender,role";

static string encodeUser(const User &u) {
    return csvEscape(u.getUsername()) + "," + csvEscape(u.getPassword()) + "," +
           csvEscape(u.getFullName()) + "," + csvEscape(u.getGender()) + "," +
           csvEscape(u.getRoleStr());
}

static User decodeUser(const vector<string> &f) {
    if (f.size() < 5) return User{};
    Role r = (f[4] == "Admin") ? Role::Admin : Role::User;
    return User(f[0], f[1], f[2], f[3], r);
}

void FileManager::saveUsers(const vector<User> &users, const string &path) {
    string out = USER_HEADER + "\n";
    for (const auto &u : users) out += encodeUser(u) + "\n";
    writeFile(path, out);
}

vector<User> FileManager::loadUsers(const string &path) {
    vector<User> result;
    string raw = readFile(path);
    if (raw.empty()) return result;
    auto lines = splitLines(raw);
    for (size_t i = 1; i < lines.size(); i++)
        result.push_back(decodeUser(csvParseLine(lines[i])));
    return result;
}

// ── Backup / Restore ─────────────────────────────────────────────────────────

void FileManager::backup() {
    ensureDataDir();
    bool s1 = copyFile("data/tasks.csv", "data/backup/tasks.csv");
    bool s2 = copyFile("data/trash.csv", "data/backup/trash.csv");
    bool s3 = copyFile("data/users.csv", "data/backup/users.csv");
    
    if (s1 || s2 || s3) cout << "  [Success] Data backed up to data/backup/\n";
    else cout << "  [Error] No files found to backup.\n";
}

void FileManager::restore() {
    bool s1 = copyFile("data/backup/tasks.csv", "data/tasks.csv");
    bool s2 = copyFile("data/backup/trash.csv", "data/trash.csv");
    bool s3 = copyFile("data/backup/users.csv", "data/users.csv");
    
    if (s1 || s2 || s3) cout << "  [Success] Data restored from backup folder.\n";
    else cout << "  [Error] Restore failed: No backup files found.\n";
}

void FileManager::clearAllTasks() {
    writeFile("data/tasks.csv", TASK_HEADER + "\n");
    writeFile("data/trash.csv", TASK_HEADER + "\n");
    cout << "  [Clean] Task and Trash files reset.\n";
}

void FileManager::clearAllUsers() {
    writeFile("data/users.csv", USER_HEADER + "\n");
    cout << "  [Clean] User database reset.\n";
}