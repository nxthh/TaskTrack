#pragma once
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

using std::cin;
using std::cout;
using std::string;

inline std::string todayDate() {
  time_t t = time(nullptr);
  tm now{};
#ifdef _WIN32
  localtime_s(&now, &t);
#else
  localtime_r(&t, &now);
#endif
  char buf[11];
  strftime(buf, sizeof(buf), "%Y-%m-%d", &now);
  return std::string(buf);
}

// ── Menu-choice helpers
// ───────────────────────────────────────────────────────

inline string choosePriority() {
  cout << "  Priority:\n"
       << "    [1] High\n"
       << "    [2] Medium\n"
       << "    [3] Low\n"
       << "  Choice: ";
  int ch;
  while (!(cin >> ch) || ch < 1 || ch > 3) {
    cin.clear();
    cin.ignore(10000, '\n');
    cout << "  Invalid. Enter 1, 2 or 3: ";
  }
  switch (ch) {
  case 1:
    return "High";
  case 2:
    return "Medium";
  default:
    return "Low";
  }
}

inline string chooseStatus() {
  cout << "  Status:\n"
       << "    [1] To-Do\n"
       << "    [2] In-Progress\n"
       << "    [3] Done\n"
       << "  Choice: ";
  int ch;
  while (!(cin >> ch) || ch < 1 || ch > 3) {
    cin.clear();
    cin.ignore(10000, '\n');
    cout << "  Invalid. Enter 1, 2 or 3: ";
  }
  switch (ch) {
  case 1:
    return "To-Do";
  case 2:
    return "In-Progress";
  default:
    return "Done";
  }
}

// Returns true if the date string is valid (YYYY-MM-DD, MM 1-12, DD 1-31)
inline bool isValidDate(const string &d) {
  if (d.size() != 10 || d[4] != '-' || d[7] != '-')
    return false;
  // All other chars must be digits
  for (int i : {0, 1, 2, 3, 5, 6, 8, 9})
    if (!isdigit((unsigned char)d[i]))
      return false;

  int mm = std::stoi(d.substr(5, 2));
  int dd = std::stoi(d.substr(8, 2));
  int yy = std::stoi(d.substr(0, 4));

  if (yy < 1)
    return false;
  if (mm < 1 || mm > 12)
    return false;
  if (dd < 1 || dd > 31)
    return false;

  // Tighter per-month check
  int daysInMonth[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (dd > daysInMonth[mm])
    return false;

  return true;
}

inline string readDueDate() {
  string d;
  while (true) {
    cout << "  Due Date (YYYY-MM-DD): ";
    cin >> d;
    if (isValidDate(d))
      return d;
    cout << "  Invalid date. Month must be 1-12, day must be 1-31, "
            "format YYYY-MM-DD.\n";
  }
}

// ── Task class
// ────────────────────────────────────────────────────────────────

class Task {
private:
  int id;
  string title;
  string description;
  string priority; // High / Medium / Low
  string status;   // To-Do / In-Progress / Done
  string dueDate;
  string createdDate;
  string assignee; // username of owner
  bool deleted = false;

public:
  Task() : id(0) {}

  // ── getters ──────────────────────────────────────────────
  int getId() const { return id; }
  string getTitle() const { return title; }
  string getDescription() const { return description; }
  string getPriority() const { return priority; }
  string getStatus() const { return status; }
  string getDueDate() const { return dueDate; }
  string getCreatedDate() const { return createdDate; }
  string getAssignee() const { return assignee; }
  bool isDeleted() const { return deleted; }

  // ── setters ──────────────────────────────────────────────
  void setId(int i) { id = i; }
  void setTitle(const string &v) { title = v; }
  void setDescription(const string &v) { description = v; }
  void setPriority(const string &v) { priority = v; }
  void setStatus(const string &v) { status = v; }
  void setDueDate(const string &v) { dueDate = v; }
  void setCreatedDate(const string &v) { createdDate = v; }
  void setAssignee(const string &v) { assignee = v; }
  void setDeleted(bool v) { deleted = v; }

  // ── helpers ──────────────────────────────────────────────
  bool isOverdue() const {
    return (status != "Done" && !dueDate.empty() && dueDate < todayDate());
  }

  // Fill from user input (used when adding a task)
  void input(const string &ownerUsername, int autoId) {
    id = autoId;
    assignee = ownerUsername;
    createdDate = todayDate();

    cout << "\n--- Add New Task ---\n";

    cout << "  Title: ";
    cin.ignore();
    getline(cin, title);

    cout << "  Description: ";
    getline(cin, description);

    priority = choosePriority();
    status = chooseStatus();
    dueDate = readDueDate();
  }

  // Edit fields interactively (press Enter to keep current value)
  void edit() {
    string inp;
    cout << "\n--- Edit Task (press Enter to keep current value) ---\n";

    auto askText = [&](const string &label, string &field) {
      cout << "  " << label << " [" << field << "]: ";
      getline(cin, inp);
      if (!inp.empty())
        field = inp;
    };

    cin.ignore();
    askText("Title", title);
    askText("Description", description);

    // Priority choice
    cout << "  Keep current priority [" << priority << "]? (y/n): ";
    char c;
    cin >> c;
    if (c == 'n' || c == 'N')
      priority = choosePriority();

    // Status choice
    cout << "  Keep current status [" << status << "]? (y/n): ";
    cin >> c;
    if (c == 'n' || c == 'N')
      status = chooseStatus();

    // Due date
    cout << "  Keep current due date [" << dueDate << "]? (y/n): ";
    cin >> c;
    if (c == 'n' || c == 'N')
      dueDate = readDueDate();
  }

  // Advance status along the pipeline
  void advanceStatus() {
    if (status == "To-Do")
      status = "In-Progress";
    else if (status == "In-Progress")
      status = "Done";
    else
      cout << "  Task is already Done.\n";
  }
};