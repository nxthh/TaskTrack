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
     * @param bannerType "MAIN", "USER", "ADMIN", or "RECOVERY"
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

    // ── Sub-menus ─────────────────────────────────────────────
    void handleTaskMenu();
    void handleDashboard();
    void handleAdminUserMenu();
    void handleAdminDataMenu();
    void handleRecoveryMenu();

public:
    // ── Core Execution ────────────────────────────────────────
    void run();
};