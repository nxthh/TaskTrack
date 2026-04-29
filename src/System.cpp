#include "System.h"
#include "FileManager.h"
#include <iostream>
#include <limits>

using namespace std;

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

// ── Auth menu ────────────────────────────────────────────────────────────────

void System::showAuthMenu() {
    cout << "\n========== TASK TRACK ==========\n";
    cout << "1. Login\n";
    cout << "2. Sign Up\n";
    cout << "0. Exit\n";
    cout << "=================================\n";
}

// ── User task sub-menu ───────────────────────────────────────────────────────

void System::handleUserTaskMenu() {
    auto* u     = auth.getUser();
    bool  admin = u->isAdmin();
    string uname = u->getUsername();

    int ch;
    do {
        cout << "\n--- Task Menu ---\n";
        cout << "1.  View All Tasks\n";
        cout << "2.  Add Task\n";
        cout << "3.  Edit Task\n";
        cout << "4.  Delete Task\n";
        cout << "5.  Advance Status (To-Do→In-Progress→Done)\n";
        cout << "6.  Search Tasks\n";
        cout << "7.  Filter by Status\n";
        cout << "8.  Filter by Priority\n";
        cout << "9.  Filter by Date\n";
        cout << "10. Sort Tasks\n";
        cout << "11. View Today's Tasks\n";
        cout << "12. View Upcoming Tasks\n";
        cout << "13. View Overdue Tasks\n";
        cout << "14. Statistics\n";
        cout << "15. Trash — View\n";
        cout << "16. Trash — Restore Task\n";
        cout << "17. Trash — Empty\n";
        cout << "0.  Back\n";
        ch = readInt("Choice: ");

        if      (ch == 1)  tasks.showAll(uname, admin);
        else if (ch == 2)  tasks.addTask(uname);
        else if (ch == 3)  tasks.editTask(readInt("  Task ID: "), uname, admin);
        else if (ch == 4)  tasks.deleteTask(readInt("  Task ID: "), uname, admin);
        else if (ch == 5)  tasks.advanceStatus(readInt("  Task ID: "), uname, admin);
        else if (ch == 6)  tasks.search(uname, admin);
        else if (ch == 7)  tasks.filterByStatus(uname, admin);
        else if (ch == 8)  tasks.filterByPriority(uname, admin);
        else if (ch == 9)  tasks.filterByDate(uname, admin);
        else if (ch == 10) tasks.sortTasks(uname, admin);
        else if (ch == 11) tasks.showToday(uname, admin);
        else if (ch == 12) tasks.showUpcoming(uname, admin);
        else if (ch == 13) tasks.showOverdue(uname, admin);
        else if (ch == 14) tasks.statistics(uname, admin);
        else if (ch == 15) tasks.showTrash();
        else if (ch == 16) tasks.restoreTask(readInt("  Task ID to restore: "));
        else if (ch == 17) tasks.emptyTrash();
    } while (ch != 0);
}

// ── Admin — user management ──────────────────────────────────────────────────

void System::handleAdminUserMenu() {
    int ch;
    do {
        cout << "\n--- User Management ---\n";
        cout << "1. View All Users\n";
        cout << "2. Search User\n";
        cout << "3. Delete User\n";
        cout << "4. Reset User Password\n";
        cout << "5. Clear All Users (keep admin)\n";
        cout << "0. Back\n";
        ch = readInt("Choice: ");

        if (ch == 1) {
            auth.viewAllUsers();
        } else if (ch == 2) {
            string kw;
            cout << "  Keyword: "; cin >> kw;
            auth.searchUser(kw);
        } else if (ch == 3) {
            string u;
            cout << "  Username to delete: "; cin >> u;
            auth.deleteUser(u);
        } else if (ch == 4) {
            string u;
            cout << "  Username: "; cin >> u;
            auth.resetPassword(u);
        } else if (ch == 5) {
            cout << "  Clear all non-admin users? (y/n): ";
            char c; cin >> c;
            if (c == 'y' || c == 'Y') auth.clearAllUsers();
        }
    } while (ch != 0);
}

// ── Admin — data management ──────────────────────────────────────────────────

void System::handleAdminDataMenu() {
    int ch;
    do {
        cout << "\n--- Data Management ---\n";
        cout << "1. Backup Data\n";
        cout << "2. Restore from Backup\n";
        cout << "3. Clear All Tasks\n";
        cout << "0. Back\n";
        ch = readInt("Choice: ");

        if      (ch == 1) FileManager::backup();
        else if (ch == 2) {
            cout << "  Restore from backup? Current data will be overwritten. (y/n): ";
            char c; cin >> c;
            if (c == 'y' || c == 'Y') {
                FileManager::restore();
                tasks.loadFromFile();
                tasks.loadTrash();
                auth.loadFromFile();
            }
        } else if (ch == 3) tasks.clearAllTasks();
    } while (ch != 0);
}

// ── Normal user menu ─────────────────────────────────────────────────────────

void System::showUserMenu() {
    auto* u = auth.getUser();
    int ch;
    do {
        cout << "\n========== TASK TRACK — " << u->getUsername() << " ==========\n";
        cout << "1. Tasks\n";
        cout << "2. Logout\n";
        cout << "===========================================\n";
        ch = readInt("Choice: ");

        if      (ch == 1) handleUserTaskMenu();
        else if (ch == 2) { auth.logout(); return; }
    } while (ch != 0 && auth.isLoggedIn());
}

// ── Admin menu ───────────────────────────────────────────────────────────────

void System::showAdminMenu() {
    auto* u = auth.getUser();
    int ch;
    do {
        cout << "\n========== TASK TRACK — ADMIN ==========\n";
        cout << "1. Tasks\n";
        cout << "2. User Management\n";
        cout << "3. Data Management\n";
        cout << "4. Logout\n";
        cout << "========================================\n";
        ch = readInt("Choice: ");

        if      (ch == 1) handleUserTaskMenu();
        else if (ch == 2) handleAdminUserMenu();
        else if (ch == 3) handleAdminDataMenu();
        else if (ch == 4) { auth.logout(); return; }
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
            if (auth.login()) {
                if (auth.getUser()->isAdmin()) showAdminMenu();
                else                           showUserMenu();
            }
        } else if (ch == 2) {
            auth.signup();
        }
    } while (ch != 0);

    cout << "\n  Goodbye!\n";
}
