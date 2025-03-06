# Linux Task Manager

This project is a custom Linux Task Manager, comprising a kernel module (`tm.c`) and a GTK-based GUI (`task_manager_ui.c`).

## Overview

This task manager provides a user-friendly interface for monitoring and managing processes on a Linux system. It leverages a kernel module for efficient data collection and presents the information through a GTK-based graphical interface.

## Features

*   **Real-time Process Monitoring:** Displays detailed process information, including PID, CPU time, memory usage, number of threads, and more.
*   **GTK GUI:** Provides a user-friendly interface for visualizing process data, with sorting and filtering options.
*   **Process Management:** Allows users to start, stop, or kill processes directly from the GUI.
*   **Kernel Module Integration:** Employs a kernel module for extended process tracking and efficient data retrieval.
*   **Lightweight and Efficient:** Utilizes the `/proc` filesystem for optimal performance.

## Installation & Setup

### 1. Clone the Repository

```bash
git clone https://github.com/aaravkohli/TaskManager.git
cd TaskManager 
```
2. Build and Load the Kernel Module (tm.c)

```bash
make
sudo insmod tm.ko
lsmod | grep tm  # Verify the module is loaded
ls /proc | grep task_manager # Verify the /proc entry exists
cat /proc/task_manager # View raw process data
```
3. Install GTK for GUI (task_manager_ui.c)

```bash
sudo apt update  # For Debian/Ubuntu-based distributions
sudo apt install libgtk-3-dev
```

For other distributions, use the appropriate package manager (e.g., yum, dnf, pacman) to install the GTK development libraries. Search for a package similar to gtk3-devel or gtk3-dev.

4. Compile and Run the GUI (task_manager_ui.c)

```bash
gcc task_manager_ui.c -o task_manager_ui `pkg-config --cflags --libs gtk+-3.0`
./task_manager_ui
```

## Unloading & Cleaning Up


```bash
sudo rmmod tm
make clean
```

## Contributing
Contributions are welcome! Feel free to submit issues, feature requests, or pull requests.

## License
MIT License
