#pragma once
#include <iostream>
#include <ctime>
#include <string>

using std::string;
using std::cout;
using std::cin;

inline std::string todayDate() {
    time_t t = time(0);
    tm now;
    localtime_s(&now, &t);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", &now);
    return std::string(buf);
}

class Task {
private:
    int    id;
    string title;
    string description;
    string priority;    // High / Medium / Low
    string status;      // To-Do / In-Progress / Done
    string dueDate;
    string createdDate;
    string assignee;    // username of owner
    bool   deleted = false;

public:
    Task() : id(0) {}

    // ── getters ──────────────────────────────────────────────
    int    getId()          const { return id; }
    string getTitle()       const { return title; }
    string getDescription() const { return description; }
    string getPriority()    const { return priority; }
    string getStatus()      const { return status; }
    string getDueDate()     const { return dueDate; }
    string getCreatedDate() const { return createdDate; }
    string getAssignee()    const { return assignee; }
    bool   isDeleted()      const { return deleted; }

    // ── setters ──────────────────────────────────────────────
    void setId(int i)                  { id = i; }
    void setTitle(const string& v)       { title = v; }
    void setDescription(const string& v) { description = v; }
    void setPriority(const string& v)    { priority = v; }
    void setStatus(const string& v)      { status = v; }
    void setDueDate(const string& v)     { dueDate = v; }
    void setCreatedDate(const string& v) { createdDate = v; }
    void setAssignee(const string& v)    { assignee = v; }
    void setDeleted(bool v)              { deleted = v; }

    // ── helpers ──────────────────────────────────────────────
    bool isOverdue() const {
        return (status != "Done" && !dueDate.empty() && dueDate < todayDate());
    }

    // Fill from user input (used when adding a task)
    void input(const string& ownerUsername, int autoId) {
        id          = autoId;
        assignee    = ownerUsername;
        createdDate = todayDate();

        cout << "\n--- Add New Task ---\n";
        cout << "Title: ";
        cin.ignore();
        getline(cin, title);

        cout << "Description: ";
        getline(cin, description);

        cout << "Priority (High / Medium / Low): ";
        cin >> priority;

        cout << "Status (To-Do / In-Progress / Done): ";
        cin >> status;

        cout << "Due Date (YYYY-MM-DD): ";
        cin >> dueDate;
    }

    // Edit fields interactively (press Enter to keep current value)
    void edit() {
        string inp;
        cout << "\n--- Edit Task (press Enter to keep current value) ---\n";

        auto ask = [&](const string& label, string& field) {
            cout << label << " [" << field << "]: ";
            getline(cin, inp);
            if (!inp.empty()) field = inp;
        };

        cin.ignore();
        ask("Title",       title);
        ask("Description", description);
        ask("Priority (High / Medium / Low)", priority);
        ask("Status (To-Do / In-Progress / Done)", status);
        ask("Due Date (YYYY-MM-DD)",  dueDate);
    }

    // Advance status along the pipeline
    void advanceStatus() {
        if      (status == "To-Do")       status = "In-Progress";
        else if (status == "In-Progress") status = "Done";
        else    cout << "  Task is already Done.\n";
    }
};