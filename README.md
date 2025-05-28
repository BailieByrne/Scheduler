🧠 Task Scheduler (C++ Multithreaded)
A lightweight, priority-based multithreaded task scheduler written in C++ as preparation for a 12-week BAE internship. The scheduler executes tasks concurrently with support for timeouts, safe interruption, and thread-safe logging.



🚀 Features:
- ✅ Priority-based task execution

- 🧵 Multithreaded execution with std::thread

- 🔒 Thread safety using std::mutex and std::atomic

- ⏱ Custom clock class with 5ms tick intervals

- ❌ Timeout and automatic task interruption

- 🛠 Supports commands like add, print, reverse, and factorial



🧪 Example Tasks
Tasks are created with:

- std::make_shared<Task>(id, priority, std::vector<std::string>{"command", "arg1", "arg2"});


Example:

- auto t = std::make_shared<Task>(1, 5, {"add", "2", "3"});
- scheduler.addTask(t);


🛠 Commands Supported
Command	Description	Example:
- add	Adds two numbers	{"add", "5", "10"}
- print	Prints a message	{"print", "Hello"}
- reverse	Reverses a string	{"reverse", "test"}
- factorial	Computes factorial (w/ timeout)	{"factorial", "5"}

Factorial Uses Recursion to allow timeouts for larger numbers
Results are stored in a Long so large numbers will result in 0 or overflows
