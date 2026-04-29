#pragma once
#include "AuthManager.h"
#include "TaskManager.h"

class System {
private:
    AuthManager auth;
    TaskManager tasks;

    void showAuthMenu();
    void showUserMenu();
    void showAdminMenu();

    void handleUserTaskMenu();
    void handleAdminUserMenu();
    void handleAdminDataMenu();

public:
    void run();
};
