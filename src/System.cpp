#include "System.h"
#include "FileManager.h"
#include <iostream>
#include <limits>
#include <vector>
#include <windows.h>
#include <tabulate/table.hpp>

using namespace std;
using namespace tabulate;

// ── UI Visual Engine ─────────────────────────────────────────────────────────

void System::renderUI(const string& bannerType, const string& subTitle, const vector<pair<string, string>>& options) {
    system("cls");
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    if (bannerType == "MAIN") {
        SetConsoleTextAttribute(h, 11); 
        cout << R"(
  ████████╗ █████╗ ███████╗██╗  ██╗     ████████╗██████╗  █████╗  ██████╗██╗  ██╗
  ╚══██╔══╝██╔══██╗██╔════╝██║ ██╔╝     ╚══██╔══╝██╔══██╗██╔══██╗██╔════╝██║ ██╔╝
     ██║   ███████║███████╗█████╔╝         ██║   ██████╔╝███████║██║     █████╔╝ 
     ██║   ██╔══██║╚════██║██╔═██╗         ██║   ██╔══██╗██╔══██║██║     ██╔═██╗ 
     ██║   ██║  ██║███████║██║  ██╗        ██║   ██║  ██║██║  ██║╚██████╗██║  ██╗
     ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝        ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝ )" << endl;
    } 
    else if (bannerType == "USER") {
        SetConsoleTextAttribute(h, 10); 
        cout << R"(
  ██████╗  █████╗ ███████╗██╗  ██╗██████╗  ██████╗  █████╗ ██████╗ ██████╗ 
  ██╔══██╗██╔══██╗██╔════╝██║  ██║██╔══██╗██╔═══██╗██╔══██╗██╔══██╗██╔══██╗
  ██║  ██║███████║███████╗███████║██████╔╝██║   ██║███████║██████╔╝██║  ██║
  ██║  ██║██╔══██║╚════██║██╔══██║██╔══██╗██║   ██║██╔══██║██╔══██╗██║  ██║
  ██████╔╝██║  ██║███████║██║  ██║██████╔╝╚██████╔╝██║  ██║██║  ██║██████╔╝
  ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═════╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝ )" << endl;
    }
    else if (bannerType == "ADMIN") {
        SetConsoleTextAttribute(h, 14); 
        cout << R"(
   █████╗ ██████╗ ███╗   ███╗██╗███╗   ██╗    ██████╗  █████╗ ███╗   ██╗███████╗██╗    
  ██╔══██╗██╔══██╗████╗ ████║██║████╗  ██║    ██╔══██╗██╔══██╗████╗  ██║██╔════╝██║    
  ███████║██║  ██║██╔████╔██║██║██╔██╗ ██║    ██████╔╝███████║██╔██╗ ██║█████╗  ██║    
  ██╔══██║██║  ██║██║╚██╔╝██║██║██║╚██╗██║    ██╔═══╝ ██╔══██║██║╚██╗██║██╔══╝  ██║    
  ██║  ██║██████╔╝██║ ╚═╝ ██║██║██║ ╚████║    ██║     ██║  ██║██║ ╚████║███████╗███████╗
  ╚═╝  ╚═╝╚═════╝ ╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝    ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚══════╝ )" << endl;
    }
    else if (bannerType == "RECOVERY") {
        SetConsoleTextAttribute(h, 12); 
        cout << R"(
   ██████╗ ███████╗ ██████╗ ██████╗ ██╗    ██╗███████╗██████╗ ██╗   ██╗
   ██╔══██╗██╔════╝██╔════╝██╔═══██╗██║    ██║██╔════╝██╔══██╗╚██╗ ██╔╝
   ██████╔╝█████╗  ██║     ██║   ██║██║    ██║█████╗  ██████╔╝ ╚████╔╝ 
   ██╔══██╗██╔══╝  ██║     ██║   ██║╚██╗  ██╔╝██╔══╝  ██╔══██╗  ╚██╔╝  
   ██║  ██║███████╗╚██████╗╚██████╔╝ ╚████╔╝ ███████╗██║  ██║   ██║   
   ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚═════╝  ╚═══╝   ╚══════╝╚═╝  ╚═╝   ╚═╝ )" << endl;
    }
    else if (bannerType == "GOODBYE") {
        SetConsoleTextAttribute(h, 4); // 
        cout << R"(
   ██████╗  ██████╗  ██████╗ ██████╗ ██████╗ ██╗   ██╗███████╗
   ██╔════╝ ██╔═══██╗██╔═══██╗██╔══██╗██╔══██╗╚██╗ ██╔╝██╔════╝
   ██║  ███╗██║   ██║██║   ██║██║  ██║██████╔╝ ╚████╔╝ █████╗  
   ██║   ██║██║   ██║██║   ██║██║  ██║██╔══██╗  ╚██╔╝  ██╔══╝  
   ╚██████╔╝╚██████╔╝╚██████╔╝██████╔╝██████╔╝   ██║   ███████╗
    ╚═════╝  ╚═════╝  ╚═════╝ ╚═════╝ ╚═════╝    ╚═╝   ╚══════╝ )" << endl;
    }

    SetConsoleTextAttribute(h, 7); 
    string line = "+-----+----------------------------------------------------+";
    cout << "\n " << line << "\n";
    printf(" | INF | %-50s |\n", subTitle.c_str());
    cout << " " << line << "\n";
    
    SetConsoleTextAttribute(h, 11); 
    printf(" | ID  | %-50s |\n", "System Action");
    SetConsoleTextAttribute(h, 7); 
    cout << " " << line << "\n";

    for (const auto& opt : options) {
        printf(" | %-3s | %-50s |\n", opt.first.c_str(), opt.second.c_str());
        cout << " " << line << "\n";
    }
    cout << endl;
}

// ── Helpers ──────────────────────────────────────────────────────────────────

void System::clearScreen() { system("cls"); }

void System::pauseScreen() {
    cout << "\n   Press Enter to continue...";
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    cin.get();
}

static void clearInput() {
    cin.clear();
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
}

static int readInt(const string& prompt) {
    int v;
    cout << prompt;
    while (!(cin >> v)) {
        clearInput();
        cout << "   Invalid input. Please enter a number: ";
    }
    return v;
}

// ── Auth Logic ───────────────────────────────────────────────────────────────

void System::showAuthMenu() {
    renderUI("MAIN", "AUTHENTICATION GATEWAY", {
        {"1", "Login to Account"},
        {"2", "Register New User"},
        {"0", "Exit"}
    });
}

// ── Dashboard Logic ──────────────────────────────────────────────────────────

void System::handleDashboard() {
    auto* u = auth.getUser();
    if(!u) return;
    bool admin = u->isAdmin();
    string owner = u->getUsername();
    clearScreen();

    int completed = tasks.completedTasks(owner, admin);
    int pending = tasks.pendingTasks(owner, admin);
    int total = admin ? tasks.totalTasks() : (completed + pending);

    Table t;
    t.add_row({"Metric", "Count"});
    t[0].format().font_style({FontStyle::bold}).font_color(Color::cyan);

    if (admin) {
        t.add_row({"System-Wide Tasks", to_string(tasks.totalTasks())});
        t.add_row({"Total Completed", to_string(tasks.totalCompletedSystemWide())});
        t.add_row({"Total Pending", to_string(tasks.totalPendingSystemWide())});
    } else {
        t.add_row({"Your Completed Tasks", to_string(completed)});
        t.add_row({"Your Pending Tasks", to_string(pending)});
    }

    t.format().border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    cout << "\n   === PERFORMANCE DASHBOARD - " << owner << " ===\n\n";
    cout << t << "\n";

    if (total > 0) {
        int barWidth = 30;
        int filled = (completed * barWidth) / total;
        cout << "   Progress: [";
        for (int i = 0; i < barWidth; i++) cout << (i < filled ? "#" : "-");
        cout << "] " << (completed * 100 / total) << "% complete\n";
    }
    pauseScreen();
}

// ── Task Management Logic ────────────────────────────────────────────────────

void System::handleTaskMenu() {
    auto* u = auth.getUser();
    if(!u) return;
    bool admin = u->isAdmin();
    string owner = u->getUsername();
    int ch;

    do {
        renderUI("USER", "TASK OPERATIONAL INTERFACE", {
            {"1", "View All Tasks"},
            {"2", "Add New Task"},
            {"3", "Edit Existing Task"},
            {"4", "Delete Task (To Trash)"},
            {"5", "Update Status (Next Step)"},
            {"6", "Search Title keyword"},
            {"7", "Sort & Filter Sub-Menu"},
            {"0", "Return to Dashboard"}
        });

        ch = readInt(" >> Choice: ");

        if (ch == 1) { clearScreen(); tasks.showAll(owner, admin); pauseScreen(); }
        else if (ch == 2) { clearScreen(); tasks.addTask(owner); pauseScreen(); }
        else if (ch == 3) { 
            clearScreen(); tasks.showAll(owner, admin); 
            tasks.editTask(readInt(" Enter Task ID: "), owner, admin); pauseScreen(); 
        }
        else if (ch == 4) { 
            clearScreen(); tasks.showAll(owner, admin); 
            tasks.deleteTask(readInt(" Enter Task ID: "), owner, admin); pauseScreen(); 
        }
        else if (ch == 5) { 
            clearScreen(); tasks.showAll(owner, admin); 
            tasks.advanceStatus(readInt(" Enter Task ID: "), owner, admin); pauseScreen(); 
        }
        else if (ch == 6) {
            clearScreen(); string kw; cout << " Search Title: "; cin.ignore(); getline(cin, kw);
            tasks.searchTasks(kw, owner, admin); pauseScreen();
        }
        else if (ch == 7) {
            clearScreen();
            cout << "\n   === SORTING & FILTERING ===\n";
            cout << "   1. Sort by Deadline (ASC)\n";
            cout << "   2. Sort by Priority (High-Low)\n";
            cout << "   3. Filter by Status\n";
            cout << "   0. Cancel\n";
            int sub = readInt("   >> Select: ");
            if(sub == 1) tasks.sortByDeadline(owner, admin);
            else if(sub == 2) tasks.sortByPriority(owner, admin);
            else if(sub == 3) {
                string stat; cout << "   Filter (To-Do / In Progress / Done): "; cin.ignore(); getline(cin, stat);
                tasks.filterByStatus(stat, owner, admin);
            }
            if (sub != 0) pauseScreen();
        }
    } while (ch != 0);
}

// ── Recovery Logic ───────────────────────────────────────────────────────────

void System::handleRecoveryMenu() {
    auto* u = auth.getUser();
    if(!u) return;
    string owner = u->getUsername();
    bool admin = u->isAdmin();
    int ch;

    do {
        renderUI("RECOVERY", "RECOVERY CENTER & TRASH", {
            {"1", "View Deleted Tasks"},
            {"2", "Restore Deleted Task"},
            {"3", "Permanent Data Wipe"},
            {"0", "Back"}
        });

        ch = readInt(" >> Choice: ");

        if (ch == 1) { 
            clearScreen(); tasks.showTrash(owner, admin); pauseScreen(); 
        }
        else if (ch == 2) { 
            clearScreen(); tasks.showTrash(owner, admin); 
            tasks.restoreTask(readInt(" ID to Restore: "), owner, admin); 
            pauseScreen(); 
        }
        else if (ch == 3) { 
            clearScreen(); tasks.showTrash(owner, admin); 
            tasks.permanentDelete(readInt(" ID to Wipe: "), owner, admin); 
            pauseScreen(); 
        }
    } while (ch != 0);
}

// ── Admin User Management ───────────────────────────────────────────────────

void System::handleAdminUserMenu() {
    int ch;
    do {
        renderUI("ADMIN", "USER ACCOUNT MANAGEMENT", {
            {"1", "View All Users"},
            {"2", "Search User Database"},
            {"3", "Delete User Account"},
            {"4", "Reset User Password"},
            {"0", "Back"}
        });

        ch = readInt(" >> Choice: ");

        if (ch == 1) { clearScreen(); auth.viewAllUsers(); pauseScreen(); }
        else if (ch == 2) { 
            clearScreen(); string kw; cout << " Keyword: "; cin >> kw; 
            auth.searchUser(kw); pauseScreen(); 
        }
        else if (ch == 3) { 
            clearScreen(); auth.viewAllUsers(); string u; cout << " Username to Delete: "; cin >> u; 
            auth.deleteUser(u); pauseScreen(); 
        }
        else if (ch == 4) { 
            clearScreen(); string u; cout << " Username: "; cin >> u; 
            auth.resetPassword(u); pauseScreen(); 
        }
    } while (ch != 0);
}

// ── Admin Data Management ───────────────────────────────────────────────────

void System::handleAdminDataMenu() {
    int ch;
    do {
        renderUI("ADMIN", "DATA PERSISTENCE & BACKUP", {
            {"1", "Manual Backup"},
            {"2", "Restore System from Backup"},
            {"3", "Wipe All Active Tasks"},
            {"0", "Back"}
        });

        ch = readInt(" >> Choice: ");

        if (ch == 1) { clearScreen(); FileManager::backup(); pauseScreen(); }
        else if (ch == 2) {
            clearScreen(); cout << " Proceed with System Restore? (y/n): "; char c; cin >> c;
            if (c == 'y' || c == 'Y') {
                FileManager::restore(); 
                tasks.loadFromFile(); 
                tasks.loadTrash(); 
                auth.loadFromFile();
                cout << "   Restore Complete.\n";
            }
            pauseScreen();
        }
        else if (ch == 3) { 
            clearScreen(); cout << " Are you sure? This wipes ALL tasks. (y/n): "; char c; cin >> c;
            if (c == 'y' || c == 'Y') tasks.clearAllTasks(); 
            pauseScreen(); 
        }
    } while (ch != 0);
}

// ── Main Access Menus ────────────────────────────────────────────────────────

void System::showUserMenu() {
    auto* u = auth.getUser();
    if (!u) return;
    int ch;
    do {
        renderUI("USER", "MAIN DASHBOARD - " + u->getUsername(), {
            {"1", "Task Management"},
            {"2", "Performance Dashboard"},
            {"3", "Recovery Hub"},
            {"4", "Sign Out"}
        });
        ch = readInt(" >> Selection: ");
        if (ch == 1) handleTaskMenu();
        else if (ch == 2) handleDashboard();
        else if (ch == 3) handleRecoveryMenu();
        else if (ch == 4) { auth.logout(); return; }
    } while (ch != 0 && auth.isLoggedIn());
}

void System::showAdminMenu() {
    int ch;
    do {
        renderUI("ADMIN", "ADMINISTRATIVE CONTROL PANEL", {
            {"1", "Task Management (All)"},
            {"2", "System Dashboard"},
            {"3", "Global Recovery Hub"},
            {"4", "User Management"},
            {"5", "Data & Backups"},
            {"6", "Sign Out"}
        });
        ch = readInt(" >> Selection: ");
        if (ch == 1) handleTaskMenu();
        else if (ch == 2) handleDashboard();
        else if (ch == 3) handleRecoveryMenu();
        else if (ch == 4) handleAdminUserMenu();
        else if (ch == 5) handleAdminDataMenu();
        else if (ch == 6) { auth.logout(); return; }
    } while (ch != 0 && auth.isLoggedIn());
}

// ── Entry point ──────────────────────────────────────────────────────────────

void System::run() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    FileManager::ensureDataDir();
    auth.loadFromFile();
    tasks.loadFromFile();
    tasks.loadTrash();

    int ch;
    do {
        showAuthMenu();
        ch = readInt(" >> Access Command: ");

        if (ch == 1) {
            clearScreen();
            if (auth.login()) {
                pauseScreen();
                if (auth.getUser()->isAdmin()) showAdminMenu();
                else showUserMenu();
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

    // Save before exit and show the new Goodbye Banner
    renderUI("GOODBYE", "SYSTEM SHUTDOWN - ALL DATA SECURED", {});
    
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 14); 
    cout << "\n   ==============================================" << endl;
    cout << "    [EXIT] Data Saved. Secure Session Closed." << endl;
    cout << "   ==============================================\n\n";
    
    // Brief sleep to let the user see the banner before the window closes
    Sleep(1500);
}
