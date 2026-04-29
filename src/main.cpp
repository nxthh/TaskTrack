#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <tabulate/table.hpp>

using namespace std;
using namespace tabulate;

// Get Today Date 
string todayDate() {
    time_t t = time(0);
    tm *now = localtime(&t);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", now);
    return string(buf);
}

// TASK 
class Task {
private:
    int id;
    string title, category, priority, status;
    string dueDate, assignee, notes;

public:
    Task() {}

    int getId() { return id; }
    string getTitle() { return title; }
    string getCategory() { return category; }
    string getPriority() { return priority; }
    string getStatus() { return status; }
    string getDueDate() { return dueDate; }
    string getAssignee() { return assignee; }
    string getNotes() { return notes; }

    bool isOverdue() {
        return (status != "Done" && dueDate < todayDate());
    }

    void input() {
        cout << "\n--- Add New Task ---\n";
        cout << "Enter ID: ";
        cin >> id;
        cin.ignore();

        cout << "Enter Title: ";
        getline(cin, title);

        cout << "Enter Category (Backend/Frontend/etc): ";
        getline(cin, category);

        cout << "Enter Priority (High/Medium/Low): ";
        cin >> priority;

        cout << "Enter Status (To-Do/In-Progress/Done): ";
        cin >> status;

        cout << "Enter Due Date (YYYY-MM-DD): ";
        cin >> dueDate;
        cin.ignore();

        cout << "Enter Assignee: ";
        getline(cin, assignee);

        cout << "Enter Notes: ";
        getline(cin, notes);
    }

    void edit() {
        cin.ignore();
        string input;

        cout << "\n--- Edit Task ---\n";

        cout << "Title (" << title << "): ";
        getline(cin, input);
        if (!input.empty()) title = input;

        cout << "Category (" << category << "): ";
        getline(cin, input);
        if (!input.empty()) category = input;

        cout << "Priority (" << priority << "): ";
        getline(cin, input);
        if (!input.empty()) priority = input;

        cout << "Status (" << status << "): ";
        getline(cin, input);
        if (!input.empty()) status = input;

        cout << "Due Date (" << dueDate << "): ";
        getline(cin, input);
        if (!input.empty()) dueDate = input;

        cout << "Assignee (" << assignee << "): ";
        getline(cin, input);
        if (!input.empty()) assignee = input;

        cout << "Notes (" << notes << "): ";
        getline(cin, input);
        if (!input.empty()) notes = input;
    }
};

//  TASK MANAGER 
class TaskManager {
private:
    vector<Task> tasks;
    Task lastDeleted;
    bool canUndo = false;

public:
    void addTask() {
        Task t;
        t.input();
        tasks.push_back(t);
        cout << "Task added successfully!\n";
    }

    void showAll() {
        Table table;
        table.add_row({"ID","Title","Status","Priority","Due","Category","Assignee"});

        for (auto &t : tasks) {
            string due = t.getDueDate();
            if (t.isOverdue()) due += " (OVERDUE)";

            table.add_row({
                to_string(t.getId()),
                t.getTitle(),
                t.getStatus(),
                t.getPriority(),
                due,
                t.getCategory(),
                t.getAssignee()
            });
        }

        table[0].format().font_style({FontStyle::bold});
        cout << table << endl;
    }

    void editTask(int id) {
        for (auto &t : tasks) {
            if (t.getId() == id) {
                t.edit();
                cout << "Task updated!\n";
                return;
            }
        }
        cout << "Task not found!\n";
    }

    void deleteTask(int id) {
        auto it = find_if(tasks.begin(), tasks.end(),
            [&](Task &t){ return t.getId() == id; });

        if (it != tasks.end()) {
            lastDeleted = *it;
            canUndo = true;
            tasks.erase(it);
            cout << "Task deleted!\n";
        } else cout << "Task not found!\n";
    }


    void undo() {
        if (canUndo) {
            tasks.push_back(lastDeleted);
            canUndo = false;
            cout << "Undo successful!\n";
        } else cout << " Nothing to undo!\n";
    }

    void search() {
        string k;
        cout << "Enter keyword: ";
        cin >> k;

        vector<Task> res;
        for (auto &t : tasks) {
            if (t.getTitle().find(k) != string::npos ||
                t.getNotes().find(k) != string::npos ||
                t.getAssignee().find(k) != string::npos)
                res.push_back(t);
        }

        showFiltered(res);
    }

    void filter() {
        string type, value;
        cout << "Filter by (status/priority/category): ";
        cin >> type;
        cout << "Enter value: ";
        cin >> value;

        vector<Task> res;
        for (auto &t : tasks) {
            if ((type=="status" && t.getStatus()==value) ||
                (type=="priority" && t.getPriority()==value) ||
                (type=="category" && t.getCategory()==value))
                res.push_back(t);
        }

        showFiltered(res);
    }

    void sortTasks() {
        string type;
        cout << "Sort by (id/due/priority/status): ";
        cin >> type;

        if (type=="id")
            sort(tasks.begin(), tasks.end(), [](Task&a,Task&b){return a.getId()<b.getId();});
        else if (type=="due")
            sort(tasks.begin(), tasks.end(), [](Task&a,Task&b){return a.getDueDate()<b.getDueDate();});
        else if (type=="priority")
            sort(tasks.begin(), tasks.end(), [](Task&a,Task&b){return a.getPriority()<b.getPriority();});
        else if (type=="status")
            sort(tasks.begin(), tasks.end(), [](Task&a,Task&b){return a.getStatus()<b.getStatus();});

        cout << "Sorted successfully!\n";
    }

    void statistics() {
        int todo=0, prog=0, done=0;

        for (auto &t : tasks) {
            if (t.getStatus()=="To-Do") todo++;
            else if (t.getStatus()=="In-Progress") prog++;
            else if (t.getStatus()=="Done") done++;
        }

        int total = tasks.size();
        int completed = done;

        cout << "\n--- STATISTICS ---\n";
        cout << "Total Tasks: " << total << endl;
        cout << "To-Do: " << todo << endl;
        cout << "In-Progress: " << prog << endl;
        cout << "Done: " << done << endl;

        int width = 30;
        int filled = total ? (completed * width / total) : 0;

        cout << "Progress: [";
        for (int i=0;i<width;i++)
            cout << (i<filled ? "#" : "-");
        cout << "] " << (total? (completed*100/total):0) << "%\n";
    }

    void showFiltered(vector<Task> list) {
        Table table;
        table.add_row({"ID","Title","Status","Priority","Due","Category","Assignee"});

        for (auto &t : list) {
            table.add_row({
                to_string(t.getId()),
                t.getTitle(),
                t.getStatus(),
                t.getPriority(),
                t.getDueDate(),
                t.getCategory(),
                t.getAssignee()
            });
        }

        cout << table << endl;
    }
};

//  MAIN 
int main() {
    TaskManager m;
    int choice;

    do {
        cout << "\n========== TODO MANAGEMENT SYSTEM ==========\n";
        cout << "1. Add Task\n";
        cout << "2. Edit Task\n";
        cout << "3. Delete Task\n";
        cout << "4. Show All Tasks\n";
        cout << "5. Search Tasks\n";
        cout << "6. Filter Tasks\n";
        cout << "7. Sort Tasks\n";
        cout << "8. View Statistics\n";
        cout << "9. Undo Delete\n";
        cout << "0. Exit\n";
        cout << "===========================================\n";
        cout << "Enter your choice: ";

        cin >> choice;

        if (choice == 1) m.addTask();

        else if (choice == 2) {
            int id;
            cout << "Enter Task ID to edit: ";
            cin >> id;
            m.editTask(id);
        }


        else if (choice == 3) {
            int id;
            cout << "Enter Task ID to delete: ";
            cin >> id;
            m.deleteTask(id);
        }

        else if (choice == 4) m.showAll();
        else if (choice == 5) m.search();
        else if (choice == 6) m.filter();
        else if (choice == 7) m.sortTasks();
        else if (choice == 8) m.statistics();
        else if (choice == 9) m.undo();

    } while (choice != 0);

    cout << " Exiting program \n";
    return 0;
}