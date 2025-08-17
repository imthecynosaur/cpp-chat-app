# C++ Multi-Threaded Chat Application

This is a command-line chat application built from the ground up in C++. The project started as a practical exercise to dive deep into modern C++ features and explore the principles of network programming and concurrent systems.

At its core, this is a classic client-server application. A central server runs in one console window, capable of managing connections from multiple clients at once. Each client, running in its own window, can send messages which are then broadcast by the server to all other connected participants. The implementation uses a multi-threaded model on both the server (one thread per client) and the client (separating network I/O from user input) to ensure the application remains responsive.

## Technical Philosophy

This project was built with a strong focus on modern C++ practices and robust software design. The goal was not just to make something that works, but to build it in a way that is safe, maintainable, and reflects professional standards.

To that end, the code makes extensive use of the **RAII (Resource Acquisition Is Initialization)** pattern for managing all resources, from WinSock API state to individual socket handles and memory allocated by C-style APIs. You'll see custom smart pointer deleters and dedicated RAII wrapper classes instead of manual cleanup calls scattered throughout the code.

Concurrency is handled using the standard C++ library (`std::thread`, `std::mutex`, `std::atomic`), with careful attention paid to preventing race conditions on shared data through proper locking. Error handling is managed through a custom exception hierarchy, which keeps the main application logic clean and clearly separate from failure cases.

The language standard is C++20 to take advantage of modern features.

## Getting Started

Here's how to get the project compiled and running on your machine.

### Prerequisites

* **Visual Studio 2019 or 2022:** You'll need the "Desktop development with C++" workload installed.
* **Git:** Required for cloning the repository.

### Build Instructions

1.  First, clone the repository to your local machine:
2.  Open the `ChatApplication.sln` solution file in Visual Studio.
3.  Set your desired build configuration (e.g., `Debug` and `x64`) from the dropdowns in the toolbar.
4.  Build the entire solution by going to `Build > Build Solution` in the main menu (or by pressing `F7`). This will compile both the `ChatServer.exe` and `ChatClient.exe` executables.

### How to Run

You'll need to run the server first, and then you can launch as many clients as you'd like.

1.  **Start the Server:**
    Navigate to the build output directory. This will be inside your solution folder, under a path like `x64/Debug/`. Double-click `ChatServer.exe`. A console window will appear, indicating that the server is listening for connections.
2.  **Start the Clients:**
    In the same directory, double-click `ChatClient.exe`. This will open a new console window for your first client. To connect more users, simply run `ChatClient.exe` again for each one. You can now type messages in any client window, and they will be broadcast to all the others.

## What's Next? (Roadmap)

While the current version provides a solid foundation for real-time communication, there are many avenues for future development to explore more advanced topics:

* **User Identity:** Implement a system for usernames and basic authentication.
* **Chat Rooms:** Allow users to create and join different channels instead of a single global chat.
* **Persistence:** Save user accounts and message history to a database like SQLite.
* **Advanced Architecture:** Refactor the server to use a more scalable, event-driven architecture with a library like Boost.Asio instead of the thread-per-client model.
* **Cross-Platform Support:** Migrate the build system from MSBuild to CMake to allow the project to be compiled on Linux and macOS.
* **Formal Testing:** Add a dedicated test project using a framework like Google Test to write unit and integration tests.
