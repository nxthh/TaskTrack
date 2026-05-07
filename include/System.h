#pragma once
#include "AuthManager.h"
#include "TaskManager.h"

class System {
private:
    AuthManager auth;
    TaskManager tasks;

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
    void run();
};