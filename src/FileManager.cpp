#include "FileManager.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


namespace fs = std::filesystem;
using std::cerr;
using std::cout;
using std::ifstream;
using std::istringstream;
using std::ofstream;
using std::ostringstream;
using std::string;
using std::to_string;
using std::vector;

// ── low-level file I/O ───────────────────────────────────────────────────────

string FileManager::readFile(const string &path) {
  ifstream f(path);
  if (!f.is_open())
    return "";
  ostringstream ss;
  ss << f.rdbuf();
  return ss.str();
}

void FileManager::writeFile(const string &path, const string &content) {
  ofstream f(path);
  if (!f.is_open()) {
    cerr << "  [FileManager] Cannot write: " << path << "\n";
    return;
  }
  f << content;
}

bool FileManager::copyFile(const string &src, const string &dst) {
  try {
    if (!fs::exists(src)) {
      cerr << "  [FileManager] Source file not found: " << src << "\n";
      return false;
    }
    fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
    return true;
  } catch (const std::exception &e) {
    cerr << "  [FileManager] Copy failed: " << src << " (" << e.what() << ")\n";
    return false;
  }
}

void FileManager::ensureDataDir() {
  fs::create_directories("data");
  fs::create_directories("data/backup");
}

// ── CSV helpers (free functions — internal to this translation unit)
// ────────── RFC-4180: fields containing comma, double-quote, or newline are
// wrapped in double-quotes; any embedded double-quote is escaped as "".

static string csvEscape(const string &field) {
  bool needsQuote =
      (field.find(',') != string::npos || field.find('"') != string::npos ||
       field.find('\n') != string::npos || field.find('\r') != string::npos);
  if (!needsQuote)
    return field;

  string out = "\"";
  for (char c : field) {
    if (c == '"')
      out += "\"\"";
    else
      out += c;
  }
  out += '"';
  return out;
}

// Parse one CSV line into fields (handles quoted fields with embedded
// commas/quotes)
static vector<string> csvParseLine(const string &line) {
  vector<string> fields;
  string field;
  bool inQuotes = false;

  for (size_t i = 0; i < line.size(); i++) {
    char c = line[i];
    if (inQuotes) {
      if (c == '"') {
        if (i + 1 < line.size() && line[i + 1] == '"') {
          field += '"';
          i++;
        } else {
          inQuotes = false;
        }
      } else {
        field += c;
      }
    } else {
      if (c == '"') {
        inQuotes = true;
      } else if (c == ',') {
        fields.push_back(field);
        field.clear();
      } else if (c == '\r') { /* skip CR */
      } else {
        field += c;
      }
    }
  }
  fields.push_back(field); // last field
  return fields;
}

// Split file content into non-empty lines
static vector<string> splitLines(const string &text) {
  vector<string> lines;
  istringstream ss(text);
  string line;
  while (getline(ss, line)) {
    if (!line.empty() && line.back() == '\r')
      line.pop_back();
    if (!line.empty())
      lines.push_back(line);
  }
  return lines;
}

// ── Tasks ────────────────────────────────────────────────────────────────────

static const string TASK_HEADER =
    "id,title,description,priority,status,dueDate,createdDate,assignee,deleted";

static string encodeTask(const Task &t) {
  return csvEscape(to_string(t.getId())) + "," + csvEscape(t.getTitle()) + "," +
         csvEscape(t.getDescription()) + "," + csvEscape(t.getPriority()) +
         "," + csvEscape(t.getStatus()) + "," + csvEscape(t.getDueDate()) +
         "," + csvEscape(t.getCreatedDate()) + "," +
         csvEscape(t.getAssignee()) + "," +
         csvEscape(t.isDeleted() ? "true" : "false");
}

static Task decodeTask(const vector<string> &f) {
  if (f.size() < 9)
    return Task{};
  Task t;
  t.setId(f[0].empty() ? 0 : stoi(f[0]));
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
  ensureDataDir();
  string out = TASK_HEADER + "\n";
  for (auto &t : tasks)
    out += encodeTask(t) + "\n";
  writeFile(path, out);
}

vector<Task> FileManager::loadTasks(const string &path) {
  vector<Task> result;
  string raw = readFile(path);
  if (raw.empty())
    return result;
  auto lines = splitLines(raw);
  for (size_t i = 1; i < lines.size(); i++) // skip header row
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
  if (f.size() < 5)
    return User{};
  Role r = (f[4] == "Admin") ? Role::Admin : Role::User;
  return User(f[0], f[1], f[2], f[3], r);
}

void FileManager::saveUsers(const vector<User> &users, const string &path) {
  ensureDataDir();
  string out = USER_HEADER + "\n";
  for (auto &u : users)
    out += encodeUser(u) + "\n";
  writeFile(path, out);
}

vector<User> FileManager::loadUsers(const string &path) {
  vector<User> result;
  string raw = readFile(path);
  if (raw.empty())
    return result;
  auto lines = splitLines(raw);
  for (size_t i = 1; i < lines.size(); i++) // skip header row
    result.push_back(decodeUser(csvParseLine(lines[i])));
  return result;
}

// ── Backup / Restore ─────────────────────────────────────────────────────────

void FileManager::backup() {
  ensureDataDir();
  copyFile("data/tasks.csv", "data/backup/tasks.csv");
  copyFile("data/trash.csv", "data/backup/trash.csv");
  copyFile("data/users.csv", "data/backup/users.csv");
  cout << "  Backup saved to data/backup/\n";
}

void FileManager::restore() {
  copyFile("data/backup/tasks.csv", "data/tasks.csv");
  copyFile("data/backup/trash.csv", "data/trash.csv");
  copyFile("data/backup/users.csv", "data/users.csv");
  cout << "  Data restored from data/backup/\n";
}

void FileManager::clearAllTasks() {
  writeFile("data/tasks.csv", TASK_HEADER + "\n");
  writeFile("data/trash.csv", TASK_HEADER + "\n");
  cout << "  All task data cleared.\n";
}

void FileManager::clearAllUsers() {
  writeFile("data/users.csv", USER_HEADER + "\n");
  cout << "  All user data cleared.\n";
}