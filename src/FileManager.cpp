/*  FileManager.cpp
    Hand-rolled JSON encode/decode — no external JSON library needed.
    Format mirrors the existing tasks.json structure and is human-readable.
*/
#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

// ── low-level helpers ────────────────────────────────────────────────────────

string FileManager::readFile(const string& path) {
    ifstream f(path);
    if (!f.is_open()) return "[]";
    ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

void FileManager::writeFile(const string& path, const string& content) {
    ofstream f(path);
    if (!f.is_open()) {
        cerr << "  [FileManager] Cannot write: " << path << "\n";
        return;
    }
    f << content;
}

void FileManager::copyFile(const string& src, const string& dst) {
    try { fs::copy_file(src, dst, fs::copy_options::overwrite_existing); }
    catch (...) { cerr << "  [FileManager] Copy failed: " << src << "\n"; }
}

void FileManager::ensureDataDir() {
    fs::create_directories("data");
    fs::create_directories("data/backup");
}

// ── JSON encode helpers (minimal, no escaping of rare chars) ─────────────────

static string jstr(const string& v) {
    // Escape backslash and double-quote
    string out;
    for (char c : v) {
        if (c == '"')  out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else out += c;
    }
    return "\"" + out + "\"";
}

static string jbool(bool b) { return b ? "true" : "false"; }
static string jint(int i)   { return to_string(i); }

// ── JSON decode helpers ───────────────────────────────────────────────────────

// Extract the raw value string for a key from a JSON object string (one level)
static string jsonVal(const string& obj, const string& key) {
    string needle = "\"" + key + "\"";
    size_t pos = obj.find(needle);
    if (pos == string::npos) return "";
    pos = obj.find(':', pos + needle.size());
    if (pos == string::npos) return "";
    pos++;
    while (pos < obj.size() && isspace(obj[pos])) pos++;
    if (pos >= obj.size()) return "";
    if (obj[pos] == '"') {
        // string value
        size_t start = pos + 1;
        size_t end = start;
        while (end < obj.size()) {
            if (obj[end] == '\\') { end += 2; continue; }
            if (obj[end] == '"')  break;
            end++;
        }
        string raw = obj.substr(start, end - start);
        // unescape
        string out;
        for (size_t i = 0; i < raw.size(); i++) {
            if (raw[i] == '\\' && i + 1 < raw.size()) {
                char n = raw[++i];
                if (n == '"')  out += '"';
                else if (n == '\\') out += '\\';
                else out += n;
            } else out += raw[i];
        }
        return out;
    } else {
        // number / bool
        size_t end = pos;
        while (end < obj.size() && obj[end] != ',' && obj[end] != '}' && obj[end] != ']')
            end++;
        string val = obj.substr(pos, end - pos);
        while (!val.empty() && isspace(val.back())) val.pop_back();
        return val;
    }
}

// Split a JSON array string into individual object strings
static vector<string> jsonArray(const string& arr) {
    vector<string> items;
    int depth = 0;
    size_t start = string::npos;
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] == '{') {
            if (depth == 0) start = i;
            depth++;
        } else if (arr[i] == '}') {
            depth--;
            if (depth == 0 && start != string::npos) {
                items.push_back(arr.substr(start, i - start + 1));
                start = string::npos;
            }
        }
    }
    return items;
}

// ── Tasks ────────────────────────────────────────────────────────────────────

static string encodeTask(const Task& t) {
    return "  {\n"
        "    \"id\": "          + jint(t.getId())           + ",\n"
        "    \"title\": "       + jstr(t.getTitle())        + ",\n"
        "    \"description\": " + jstr(t.getDescription())  + ",\n"
        "    \"category\": "    + jstr(t.getCategory())     + ",\n"
        "    \"priority\": "    + jstr(t.getPriority())     + ",\n"
        "    \"status\": "      + jstr(t.getStatus())       + ",\n"
        "    \"dueDate\": "     + jstr(t.getDueDate())      + ",\n"
        "    \"createdDate\": " + jstr(t.getCreatedDate())  + ",\n"
        "    \"assignee\": "    + jstr(t.getAssignee())     + ",\n"
        "    \"deleted\": "     + jbool(t.isDeleted())      + "\n"
        "  }";
}

static Task decodeTask(const string& obj) {
    Task t;
    t.setId(stoi(jsonVal(obj, "id").empty() ? "0" : jsonVal(obj, "id")));
    t.setTitle(jsonVal(obj, "title"));
    t.setDescription(jsonVal(obj, "description"));
    t.setCategory(jsonVal(obj, "category"));
    t.setPriority(jsonVal(obj, "priority"));
    t.setStatus(jsonVal(obj, "status"));
    t.setDueDate(jsonVal(obj, "dueDate"));
    t.setCreatedDate(jsonVal(obj, "createdDate"));
    t.setAssignee(jsonVal(obj, "assignee"));
    t.setDeleted(jsonVal(obj, "deleted") == "true");
    return t;
}

void FileManager::saveTasks(const vector<Task>& tasks, const string& path) {
    ensureDataDir();
    string out = "[\n";
    for (size_t i = 0; i < tasks.size(); i++) {
        out += encodeTask(tasks[i]);
        if (i + 1 < tasks.size()) out += ",";
        out += "\n";
    }
    out += "]\n";
    writeFile(path, out);
}

vector<Task> FileManager::loadTasks(const string& path) {
    vector<Task> result;
    string raw = readFile(path);
    for (auto& obj : jsonArray(raw))
        result.push_back(decodeTask(obj));
    return result;
}

// ── Trash ────────────────────────────────────────────────────────────────────

void FileManager::saveTrash(const vector<Task>& trash, const string& path) {
    saveTasks(trash, path);   // same format
}

vector<Task> FileManager::loadTrash(const string& path) {
    return loadTasks(path);
}

// ── Users ────────────────────────────────────────────────────────────────────

static string encodeUser(const User& u) {
    return "  {\n"
        "    \"username\": " + jstr(u.getUsername()) + ",\n"
        "    \"password\": " + jstr(u.getPassword()) + ",\n"
        "    \"fullName\": " + jstr(u.getFullName()) + ",\n"
        "    \"gender\": "   + jstr(u.getGender())   + ",\n"
        "    \"role\": "     + jstr(u.getRoleStr())  + "\n"
        "  }";
}

static User decodeUser(const string& obj) {
    string roleStr = jsonVal(obj, "role");
    Role r = (roleStr == "Admin") ? Role::Admin : Role::User;
    return User(
        jsonVal(obj, "username"),
        jsonVal(obj, "password"),
        jsonVal(obj, "fullName"),
        jsonVal(obj, "gender"),
        r
    );
}

void FileManager::saveUsers(const vector<User>& users, const string& path) {
    ensureDataDir();
    string out = "[\n";
    for (size_t i = 0; i < users.size(); i++) {
        out += encodeUser(users[i]);
        if (i + 1 < users.size()) out += ",";
        out += "\n";
    }
    out += "]\n";
    writeFile(path, out);
}

vector<User> FileManager::loadUsers(const string& path) {
    vector<User> result;
    string raw = readFile(path);
    for (auto& obj : jsonArray(raw))
        result.push_back(decodeUser(obj));
    return result;
}

// ── Backup / Restore ─────────────────────────────────────────────────────────

void FileManager::backup() {
    ensureDataDir();
    copyFile("data/tasks.json", "data/backup/tasks.json");
    copyFile("data/trash.json", "data/backup/trash.json");
    copyFile("data/users.json", "data/backup/users.json");
    cout << "  Backup saved to data/backup/\n";
}

void FileManager::restore() {
    copyFile("data/backup/tasks.json", "data/tasks.json");
    copyFile("data/backup/trash.json", "data/trash.json");
    copyFile("data/backup/users.json", "data/users.json");
    cout << "  Data restored from data/backup/\n";
}

void FileManager::clearAllTasks() {
    writeFile("data/tasks.json", "[]\n");
    writeFile("data/trash.json", "[]\n");
    cout << "  All task data cleared.\n";
}

void FileManager::clearAllUsers() {
    writeFile("data/users.json", "[]\n");
    cout << "  All user data cleared.\n";
}
