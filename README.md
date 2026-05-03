# TaskTrack

TaskTrack is a robust, console-based task management system written in C++. It offers a structured way to manage personal productivity through a Command Line Interface (CLI), featuring multi-user support, role-based access control, and persistent data storage.

---

## Key Features

### 1. User Management

- **Authentication:** Secure Login and Sign-Up system.
    
- **Role-Based Access Control (RBAC):**
    
    - **Normal User:** Manage personal tasks and track individual progress.
        
    - **Admin:** Full system oversight, including the ability to view, delete, or search for users and reset passwords.
        

### 2. Task Management

- **Task Properties:** Tracks ID, Title, Description, Deadline, Priority (Low/Medium/High), and Status.
    
- **Workflow Tracking:** Move tasks through a lifecycle: `To-Do` → `In Progress` → `Done`.
    
- **Smart Filtering & Views:**
    
    - Filter by Status, Priority, or Date.
        
    - Dedicated views for **Today’s Tasks**, **Upcoming**, and **Overdue** items.
        
    - Keyword search across titles and descriptions.
        

### 3. Data & System Security

- **Persistent Storage:** Data is saved to and loaded from local files (TXT/CSV/JSON) automatically.
    
- **Backup & Recovery:** Manual backup and restore functionality to prevent data loss.
    
- **Soft Delete:** Includes a "Trash" system to restore accidentally deleted tasks.
    
- **Admin Tools:** Global data clearing options with safety confirmations.
    

### 4. Statistics Dashboard

Provides a quick overview of productivity metrics:

- Total Tasks vs. Completed Tasks
    
- Pending and Overdue task counts
    

---

## Technical Overview

### Class Structure

- **`User` Class:** Handles credentials, profile details, and role assignments.
    
- **`Task` Class:** Manages task metadata, auto-incrementing IDs, and status updates.
    
- **`FileManager`:** Handles I/O operations for data persistence and backups.
    

### Requirements

- C++ Compiler (GCC/Clang/MSVC)
    
- Standard Template Library (STL)
    

---

## How to Run

1. **Clone the repository:**
    
    Bash
    
    ```
    git clone https://github.com/nxthh/TaskTrack.git
    ```
    
2. **Compile the source code:**
    
    Bash
    
    ```
    g++ main.cpp -o TaskTrack
    ```
    
3. **Execute the program:**
    
    Bash
    
    ```
    ./TaskTrack
    
    ```
    

```

---

## Usage Note
> **Initial Setup:** Upon first launch, you will be prompted to create an account. The first user registered can be assigned the **Admin** role to access system-wide management features.
```
