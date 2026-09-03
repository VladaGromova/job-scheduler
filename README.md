# Job Scheduler

A background task queuing and execution system written in C++17. The program accepts tasks of different types (computational, file operations, simulated data downloading), queues them by priority or schedule, and executes them in the background using a pool of worker threads.

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Requirements](#requirements)
- [Building](#building)
- [Running](#running)
- [Configuration File](#configuration-file)
- [Task Definition Format](#task-definition-format)
- [Console Commands](#console-commands)
- [Task Statuses](#task-statuses)
- [Tests](#tests)
- [Project Structure](#project-structure)
- [C++/STL Features Used](#cstl-features-used)

## Features

- Defining tasks of three types: computational, file operations, simulated data downloading.
- Queuing tasks by priority (1-10) and schedule (one-time, scheduled, cyclic).
- Executing tasks in parallel using a configurable thread pool.
- Automatic retrying of failed tasks, with a configurable number of attempts.
- Real-time task status monitoring (scheduled, waiting, running, finished, failed, cancelled).
- Cancelling tasks that are waiting or currently running.
- Logging program activity and generating per-task reports.
- Loading tasks from a text file or entering them manually via the console, with the ability to add further tasks while the program is running.

## Architecture

The program consists of seven main modules:

| Module | Responsibility                                                                                    |
|---|---------------------------------------------------------------------------------------------------|
| **ConsoleHandler** | User interaction: loading tasks, the command loop.                                                |
| **Scheduler** | Central management of task queues (ready to run / waiting on schedule) and their statuses.        |
| **Executor** | Pool of worker threads executing tasks pulled from the Scheduler.                                 |
| **TaskFactory** | Creates concrete task objects from a command.                                   |
| **Task** (and subclasses) | Represents a single task; polymorphic execution logic in `ComputeTask`, `FileTask`, `DownloadTask`. |
| **Logger** | Writes logs to a file and generates reports.                                                      |
| **ConfigManager** | Loads program settings from a configuration file.                                                 |

### Threads and Synchronization

The program uses three groups of threads running concurrently:

1. **Main thread** - handles the console (`ConsoleHandler`).
2. **Scheduler thread** - monitors the schedule (`schedulerLoop`).
3. **Executor thread pool** - executes tasks (`worker`).

Access to shared data structures (`allTasks`, `readyTasks`, `scheduledTasks`) is protected by separate mutexes, and waiting for new tasks is implemented with `std::condition_variable` (no busy-waiting).

## Requirements

- A compiler supporting C++17 (tested with Apple Clang 17).
- CMake ≥ 3.15.
- OS: macOS / Linux (tested on macOS; executable paths require a `./` prefix).

## Building

```bash
mkdir -p cmake-build-debug
cd cmake-build-debug
cmake ..
cmake --build . --target job_scheduler
```

The program can also be built and run directly from CLion (the project is set up as a standard CMake application).

## Running

```bash
./job_scheduler [path_to_config]
```

If no configuration file path is given, the program looks for `config.txt` in the current directory. If the configuration file is missing the program starts with default values.

On startup, the program asks how to load the initial set of tasks:

```
Provide tasks: 1 - from file, 2 - manually. Press q to finish the program.
```

## Configuration File

A text file containing three space-separated values:

```
<thread_count> <log_file> <default_retry_count>
```

Example:

```
5 logs.txt 3
```

| Field | Meaning | Default |
|---|---|---|
| Thread count | Maximum number of parallel tasks | 1 |
| Log file | Path of the file logs are written to | `default_logs.txt` |
| Default retry count | Number of retries after a failed attempt | 1 |

## Task Definition Format

```
<filename> <type> <priority> [retries] [C/H] [time...]
```

| Field | Description                                                                                                                       |
|---|-----------------------------------------------------------------------------------------------------------------------------------|
| `filename` | Name of the executable file (e.g. `file_copying.exe`).                                                                            |
| `type` | `1` - computational, `2` - file operations, `3` - simulated data downloading.                                                     |
| `priority` | Integer 1-10 (10 = highest priority).                                                                                             |
| `retries` *(optional)* | Maximum number of retries after failure. If omitted, the default value from the configuration is used.                            |
| `C` / `H` *(optional)* | Schedule mode: `C` - cyclic, `H` - one-time run at a scheduled time. Omitting it means the task runs once, immediately.           |
| `time...` | For `H`: the run date and time (`YYYY-MM-DD_HH:MM`). For `C`: the first-run date and time, plus the cycle length in milliseconds. |

### Examples

```
file_copying.exe 2 5
```
Runs `file_copying.exe` once, type "file operations", priority 5.

```
app_downloading.exe 3 7 5
```
Runs once, type "data downloading", priority 7, up to 5 retries.

```
generate_invoice.exe 2 8 2 H 2026-06-01_12:00
```
Runs once at the scheduled time of 12:00 on June 1, 2026, priority 8, up to 2 retries.

```
track_tasks.exe 2 4 3 C 2026-06-01_00:00 60000
```
Cyclic task, first run on June 1, 2026 at midnight, repeating every 60,000 ms (1 minute).

**Note:** tasks in `H` (SCHEDULED) mode with an execution time in the past are immediately marked `FAILED`. Cyclic tasks (`C`) with a first-run time in the past are executed immediately, and the next cycle is timed from the moment the current run finishes.

## Console Commands

Once the initial batch of tasks is loaded, the program enters interactive mode:

| Command              | Description |
|----------------------|---|
| `status`             | Shows the status of all tasks. |
| `status <name>`      | Shows the status of a single task. |
| `status -s <STATUS>` | Shows tasks with the given status. |
| `cancel <name>`      | Cancels a waiting or running task. |
| `report <name>`      | Generates a report (`<name>_report.txt`) with the logs for a task. |
| `task <definition>`  | Adds a new task on the fly, using the format described above. |
| `q`                  | Exits the program. |

## Task Statuses

```
SCHEDULED   - waiting for its scheduled run time
WAITING     - ready to run, waiting for a free thread
RUNNING     - currently executing
FINISHED    - completed successfully
FAILED      - completed with failure (retries exhausted)
CANCELLED   - cancelled by the user
```

## Tests

Unit tests written with [Catch2](https://github.com/catchorg/Catch2), fetched automatically via `FetchContent`. They cover enum conversions, date parsing, task creation through `TaskFactory`, and the synchronous logic of `Scheduler` (adding, cancelling, filtering by status).

```bash
cd cmake-build-debug
cmake --build . --target job_scheduler_tests
./job_scheduler_tests
```

or via CTest:

```bash
ctest --output-on-failure
```

## Project Structure

```
include/
  enums/           - enum types (TaskStatus, TaskMode, Command, InputMode)
  helpers/         - helper functions (enum↔string conversions, date parsing, comparators)
  tasks/           - the Task class hierarchy (Task, ComputeTask, FileTask, DownloadTask)
  ConfigManager.hpp, ConsoleHandler.hpp, Executor.hpp,
  Logger.hpp, LogMessage.hpp, Scheduler.hpp, TaskFactory.hpp
src/
  corresponding .cpp files
tests/
  unit tests (Catch2)
```

## C++/STL Features Used

- **STL**: `std::priority_queue`, `std::map`, `std::vector`, `std::unique_ptr`, `std::optional`, `std::atomic`, `std::mutex`, `std::condition_variable`, `std::thread`, `std::chrono`.
- **Polymorphism**: an abstract `Task` class with a pure virtual `execute()` method, implemented by `ComputeTask`, `FileTask`, `DownloadTask`.
- **RAII**: memory management via `std::unique_ptr` (the Scheduler is the sole owner of `Task` objects), lock management via `std::lock_guard`/`std::unique_lock`.
- **Multithreading**: a worker thread pool, a schedule-monitoring thread, synchronization without busy-waiting.
- **Dependency Injection**: all dependencies (Logger, Scheduler, TaskFactory) are injected via references in constructors, instead of using global state.
