# Thread_Pool
 🧵 A lightweight, singleton-based C++ thread pool with task queuing and future-based result handling. Built for extensibility and performance. Ideal for multithreaded system development and game engine internals.

# abstd::thread_pool

**abstd::thread_pool** is a minimal yet powerful C++ thread pool built to scale multithreaded applications efficiently. Designed with simplicity and future extensibility in mind, it uses a singleton pattern and leverages C++ standard threading tools under the hood.

This library is especially useful for systems like game engines, real-time tools, or multithreaded batch processing, where controlled parallelism and task coordination are critical.

---

## 🚀 Features

- ✨ **Singleton Pattern** – One global instance across your app
- ⚡ **Auto-Scaling Threads** – Uses `std::thread::hardware_concurrency()` by default
- 🧠 **Task Queueing** – Add tasks via `std::function<void()>`
- 🧵 **Thread-Safe** – Internally synchronized using `std::mutex` and `std::condition_variable`
- 🔄 **Graceful Shutdown** – Support for both `terminate()` and `wait()` shutdown modes
- 📦 **Futures Support** – `add_call()` returns `std::future<T>` for async result handling
- 🧩 **Batch Task Submission** – `add_calls()` supports tuple-style multiple submissions

---

## 🛠️ Usage

```cpp
#include "thread_pool.hpp"

auto* pool = abstd::thread_pool::get_thread_pool();

// Add a task with return value
auto future = pool->add_call([](int a, int b) {
    return a + b;
}, 2, 3);

if (future.has_value())
    std::cout << "Result: " << future.value().get() << "\n";

// Shutdown the thread pool (optional)
abstd::thread_pool::destroy(); // or destroy(true) to wait for all tasks
```

---

## 🔄 Lifecycle Methods

```cpp
thread_pool::get_thread_pool();    // Access singleton instance
thread_pool::terminate();         // Stops processing and clears the queue
thread_pool::wait();              // Blocks until all tasks finish, then shuts down
thread_pool::destroy();           // Deletes the singleton instance
```

---

## 📊 Monitoring

```cpp
pool->get_active_task_count(); // Running right now
pool->get_queue_task_count();  // Still waiting to run
```

---

## 🧩 Planned Features

- **Task Prioritization** – Priority queues to control execution order
- **Thread Affinity Control** – Assign tasks to specific cores
- **Timeouts / Cancellations** – More robust task management
- **Metrics + Debug View** – Track throughput, idle time, etc.

---

## 📚 Requirements

- C++17 or newer
- No third-party dependencies – fully STL-based

## 👤 Author

**Abhijit Biswas** – @gamdevAbhi
Game Developer | Systems Programmer | Tooling Enthusiast