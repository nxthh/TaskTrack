#pragma once
#include "AuthManager.h"
#include "TaskManager.h"
#include <string>
#include <vector>

class System {
private:
    AuthManager auth;
    TaskManager tasks;

    // ── UI Visual Engine ──────────────────────────────────────
    /**
     * Renders the Patorjk ASCII banners and styled menu boxes.
     * @param bannerType "MAIN", "USER", "ADMIN", "RECOVERY", or "EXIT"
     * @param subTitle The text displayed in the INFO section of the box
     * @param options A list of {ID, Description} pairs for the menu
     */
    void renderUI(const std::string& bannerType, 
                  const std::string& subTitle, 
                  const std::vector<std::pair<std::string, std::string>>& options);

    // ── UI helpers ────────────────────────────────────────────
    static void clearScreen();
    static void pauseScreen();

    // ── Menus ─────────────────────────────────────────────────
    void showAuthMenu();
    void showUserMenu();
    void showAdminMenu();

    // ── Sub-menus & Logic Handlers ─────────────────────────────
    void handleTaskMenu();      // Task Creation, Edit, Delete, Update
    void handleDashboard();     // Progress bars and stats
    void handleAdminUserMenu(); // User management (Admin only)
    void handleAdminDataMenu(); // Backup & Restore (Admin only)
    void handleRecoveryMenu();  // Trash system (View, Restore, Wipe)
    
    /**
     * New: Handles the sub-logic for Sorting and Filtering tasks
     * to satisfy requirements for Deadline/Priority/Status.
     */
    void handleSortFilterMenu(); 

public:
    // ── Core Execution ────────────────────────────────────────
    /**
     * Initializes console settings (UTF-8), loads data from files,
     * and starts the main application loop.
     */
    void run();
};