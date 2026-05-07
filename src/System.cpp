#include "System.h"
#include "FileManager.h"
#include <iostream>
#include <limits>
#include <vector>
#include <tabulate/table.hpp>

using namespace std;
using namespace tabulate;

// ── Platform clear-screen ─────────────────────────────────────────────────────

void System::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void System::pauseScreen() {
    cout << "\n  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// ── Input helpers ─────────────────────────────────────────────────────────────

static void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static int readInt(const string& prompt) {
    int v;
    cout << prompt;

    while (!(cin >> v)) {
        clearInput();
        cout << "  Please enter a number: ";
    }

    return v;
}

// ── Menu helper ──────────────────────────────────────────────────────────────

static void showMenu(const string& title,
                     const vector<pair<string, string>>& items) {

    Table menu;

    // Title
    menu.add_row({title});
    menu[0][0]
        .format()
        .font_style({FontStyle::bold})
        .font_color(Color::cyan)
        .font_align(FontAlign::center);

    // Items
    for (const auto& item : items) {
        menu.add_row({item.first + ". " + item.second});
    }

    // Style
   menu.format()
    .border_top("-")
    .border_bottom("-")
    .border_left("|")
    .border_right("|")
    .corner("+")
    .width(36);

    cout << menu << "\n";
}

// ── Auth menu ─────────────────────────────────────────────────────────────────

void System::showAuthMenu() {
    clearScreen();

    showMenu("TASK TRACK", {
        {"1", "Login"},
        {"2", "Sign Up"},
        {"0", "Exit"}
    });
}

// ── Dashboard ─────────────────────────────────────────────────────────────────

void System::handleDashboard() {
    auto* u      = auth.getUser();
    bool admin   = u->isAdmin();
    string owner = u->getUsername();

    clearScreen();

    int completed = tasks.completedTasks(owner, admin);
    int pending   = tasks.pendingTasks(owner, admin);
    int total     = admin ? tasks.totalTasks() : (completed + pending);

    Table t;

    t.add_row({"Metric", "Count"});
    t[0].format()
        .font_style({FontStyle::bold})
        .font_color(Color::cyan);

    if (admin) {
        t.add_row({"Total Tasks (system-wide)", to_string(tasks.totalTasks())});
        t.add_row({"Completed Tasks (yours)", to_string(completed)});
        t.add_row({"Pending Tasks (yours)", to_string(pending)});
    } else {
        t.add_row({"Completed Tasks", to_string(completed)});
        t.add_row({"Pending Tasks", to_string(pending)});
    }

   t.format()
    .border_top("-")
    .border_bottom("-")
    .border_left("|")
    .border_right("|")
    .corner("+");

    cout << "\n=== Dashboard - " << owner << " ===\n\n";
    cout << t << "\n";

    // Progress bar
    if (total > 0) {
        int barWidth = 30;
        int filled   = completed * barWidth / total;

        cout << "Progress: [";

        for (int i = 0; i < barWidth; i++) {
            cout << (i < filled ? "#" : "-");
        }

        cout << "] "
             << (completed * 100 / total)
             << "% complete\n";
    }

    pauseScreen();
}

// ── Task menu ─────────────────────────────────────────────────────────────────

void System::handleTaskMenu() {
    auto* u      = auth.getUser();
    bool admin   = u->isAdmin();
    string owner = u->getUsername();

    int ch;

    do {
        clearScreen();

        showMenu("TASK MENU", {
            {"1", "View All Tasks"},
            {"2", "Add Task"},
            {"3", "Edit Task"},
            {"4", "Delete Task"},
            {"5", "Update Status"},
            {"0", "Back"}
        });

        ch = readInt("Choice: ");

        if (ch == 1) {
            clearScreen();
            tasks.showAll(owner, admin);
            pauseScreen();
        }
        else if (ch == 2) {
            clearScreen();
            tasks.addTask(owner);
            pauseScreen();
        }
        else if (ch == 3) {
            clearScreen();
            tasks.showAll(owner, admin);

            tasks.editTask(
                readInt("  Task ID to edit: "),
                owner,
                admin
            );

            pauseScreen();
        }
        else if (ch == 4) {
            clearScreen();
            tasks.showAll(owner, admin);

            tasks.deleteTask(
                readInt("  Task ID to delete: "),
                owner,
                admin
            );

            pauseScreen();
        }
        else if (ch == 5) {
            clearScreen();
            tasks.showAll(owner, admin);

            tasks.advanceStatus(
                readInt("  Task ID to advance: "),
                owner,
                admin
            );

            pauseScreen();
        }

    } while (ch != 0);
}

// ── Recovery menu ─────────────────────────────────────────────────────────────

void System::handleRecoveryMenu() {
    int ch;

    do {
        clearScreen();

        showMenu("RECOVERY", {
            {"1", "View Deleted Tasks"},
            {"2", "Restore Task"},
            {"3", "Permanently Delete Task"},
            {"0", "Back"}
        });

        ch = readInt("Choice: ");

        if (ch == 1) {
            clearScreen();
            tasks.showTrash();
            pauseScreen();
        }
        else if (ch == 2) {
            clearScreen();
            tasks.showTrash();

            tasks.restoreTask(
                readInt("  Task ID to restore: ")
            );

            pauseScreen();
        }
        else if (ch == 3) {
            clearScreen();
            tasks.showTrash();

            tasks.permanentDelete(
                readInt("  Task ID to permanently delete: ")
            );

            pauseScreen();
        }

    } while (ch != 0);
}

// ── Admin - user management ──────────────────────────────────────────────────

void System::handleAdminUserMenu() {
    int ch;

    do {
        clearScreen();

        showMenu("USER MANAGEMENT", {
            {"1", "View All Users"},
            {"2", "Search User"},
            {"3", "Delete User"},
            {"4", "Reset User Password"},
            {"5", "Clear All Users"},
            {"0", "Back"}
        });

        ch = readInt("Choice: ");

        if (ch == 1) {
            clearScreen();
            auth.viewAllUsers();
            pauseScreen();
        }
        else if (ch == 2) {
            clearScreen();

            string kw;

            cout << "  Keyword: ";
            cin >> kw;

            auth.searchUser(kw);

            pauseScreen();
        }
        else if (ch == 3) {
            clearScreen();

            auth.viewAllUsers();

            string u;

            cout << "  Username to delete: ";
            cin >> u;

            auth.deleteUser(u);

            pauseScreen();
        }
        else if (ch == 4) {
            clearScreen();

            string u;

            cout << "  Username: ";
            cin >> u;

            auth.resetPassword(u);

            pauseScreen();
        }
        else if (ch == 5) {
            clearScreen();

            cout << "  Clear all non-admin users? (y/n): ";

            char c;
            cin >> c;

            if (c == 'y' || c == 'Y') {
                auth.clearAllUsers();
            }

            pauseScreen();
        }

    } while (ch != 0);
}

// ── Admin - data management ──────────────────────────────────────────────────

void System::handleAdminDataMenu() {
    int ch;

    do {
        clearScreen();

        showMenu("DATA MANAGEMENT", {
            {"1", "Backup Data"},
            {"2", "Restore from Backup"},
            {"3", "Clear All Tasks"},
            {"0", "Back"}
        });

        ch = readInt("Choice: ");

        if (ch == 1) {
            clearScreen();

            FileManager::backup();

            pauseScreen();
        }
        else if (ch == 2) {
            clearScreen();

            cout << "  Restore from backup? "
                 << "Current data will be overwritten. (y/n): ";

            char c;
            cin >> c;

            if (c == 'y' || c == 'Y') {
                FileManager::restore();

                tasks.loadFromFile();
                tasks.loadTrash();
                auth.loadFromFile();
            }

            pauseScreen();
        }
        else if (ch == 3) {
            clearScreen();

            tasks.clearAllTasks();

            pauseScreen();
        }

    } while (ch != 0);
}

// ── Normal user menu ─────────────────────────────────────────────────────────

void System::showUserMenu() {
    auto* u = auth.getUser();

    int ch;

    do {
        clearScreen();

        showMenu("TASK TRACK - " + u->getUsername(), {
            {"1", "Tasks"},
            {"2", "Dashboard"},
            {"3", "Recovery"},
            {"4", "Logout"}
        });

        ch = readInt("Choice: ");

        if (ch == 1) {
            handleTaskMenu();
        }
        else if (ch == 2) {
            handleDashboard();
        }
        else if (ch == 3) {
            handleRecoveryMenu();
        }
        else if (ch == 4) {
            auth.logout();
            return;
        }

    } while (ch != 0 && auth.isLoggedIn());
}

// ── Admin menu ───────────────────────────────────────────────────────────────

void System::showAdminMenu() {
    int ch;

    do {
        clearScreen();

        showMenu("TASK TRACK - ADMIN", {
            {"1", "Tasks"},
            {"2", "Dashboard"},
            {"3", "Recovery"},
            {"4", "User Management"},
            {"5", "Data Management"},
            {"6", "Logout"}
        });

        ch = readInt("Choice: ");

        if (ch == 1) {
            handleTaskMenu();
        }
        else if (ch == 2) {
            handleDashboard();
        }
        else if (ch == 3) {
            handleRecoveryMenu();
        }
        else if (ch == 4) {
            handleAdminUserMenu();
        }
        else if (ch == 5) {
            handleAdminDataMenu();
        }
        else if (ch == 6) {
            auth.logout();
            return;
        }

    } while (ch != 0 && auth.isLoggedIn());
}

// ── Entry point ──────────────────────────────────────────────────────────────

void System::run() {
    FileManager::ensureDataDir();

    auth.loadFromFile();
    tasks.loadFromFile();
    tasks.loadTrash();

    int ch;

    do {
        showAuthMenu();

        ch = readInt("Choice: ");

        if (ch == 1) {
            clearScreen();

            if (auth.login()) {
                pauseScreen();

                if (auth.getUser()->isAdmin()) {
                    showAdminMenu();
                } else {
                    showUserMenu();
                }
            } else {
                pauseScreen();
            }
        }
        else if (ch == 2) {
            clearScreen();

            auth.signup();

            pauseScreen();
        }

    } while (ch != 0);

    clearScreen();

    cout << "\n  Goodbye!\n\n";
}