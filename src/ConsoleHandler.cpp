#include "ConsoleHandler.hpp"

#include "helpers/DateTimeUtils.hpp"
#include "helpers/EnumUtils.hpp"

#include <fstream>
#include <iostream>

namespace {
constexpr int COL_ID_WIDTH = 28;
constexpr int COL_STATUS_WIDTH = 12;
constexpr int COL_PRIO_WIDTH = 6;
constexpr int COL_MODE_WIDTH = 5;
constexpr int COL_EXEC_TIME_WIDTH = 18;
constexpr int COL_CYCLE_WIDTH = 10;
constexpr int TABLE_WIDTH = COL_ID_WIDTH + COL_STATUS_WIDTH + COL_PRIO_WIDTH +
                              COL_MODE_WIDTH + COL_EXEC_TIME_WIDTH + COL_CYCLE_WIDTH;

template <typename T>
    void printCol(const T& value, int width) {
  std::cout << std::left << std::setw(width) << value;
}

void printTaskTableHeader() {
  std::cout << std::left
             << std::setw(COL_ID_WIDTH) << "TASK ID"
             << std::setw(COL_STATUS_WIDTH) << "STATUS"
             << std::setw(COL_PRIO_WIDTH) << "PRIO"
             << std::setw(COL_MODE_WIDTH) << "MODE"
             << std::setw(COL_EXEC_TIME_WIDTH) << "EXEC TIME"
             << std::setw(COL_CYCLE_WIDTH) << "CYCLE(ms)"
             << "\n";
  std::cout << std::string(TABLE_WIDTH, '-') << std::endl;
}

void printTaskRow(const TaskInfo& info) {
  std::string execTimeStr = (info.mode != TaskMode::ONCE)
        ? DateTimeUtils::formatDateTime(info.nextExecutionTime) : "-";
  std::string cycleStr = (info.mode == TaskMode::CYCLIC)
      ? std::to_string(info.cycleInterval.count()) : "-";

  printCol(info.id, COL_ID_WIDTH);
  printCol(EnumUtils::taskStatusToString(info.status), COL_STATUS_WIDTH);
  printCol(info.priority, COL_PRIO_WIDTH);
  printCol(EnumUtils::taskModeToShortString(info.mode), COL_MODE_WIDTH);
  printCol(execTimeStr, COL_EXEC_TIME_WIDTH);
  printCol(cycleStr, COL_CYCLE_WIDTH);
  std::cout << "\n";
}
}

bool ConsoleHandler::getTasks() {
  while (true) {
    std::cout
        << "Provide tasks: 1 - from file, 2 - manually. Press q to finish "
           "the program.\n";

    std::string mode;
    if (!std::getline(std::cin, mode)) {
      return false;
    }

    switch (EnumUtils::stringToInputMode(mode)) {
    case InputMode::FROM_FILE:
      if (loadTasksFromFile()) return true;
      break;
    case InputMode::MANUAL:
      if (parseInput()) return true;
      break;
    case InputMode::QUIT:
      return false;
    case InputMode::UNKNOWN:
      std::cout << "Invalid choice! Please type 1, 2 or q to exit.\n\n";
      break;
    }
  }
}

bool ConsoleHandler::addTaskFromLine(const std::string &line) const {
  if (auto task = taskFactory.createTaskFromLine(line)) {
    std::string id = task->getId();
    scheduler.addTask(std::move(task));
    return true;
  }
  std::cout << "Invalid task format, skipped: " << line << "\n";
  return false;
}

void ConsoleHandler::printStatusTable(const std::vector<TaskInfo> &tasks,
                                      const std::string &message) const {
  if (tasks.empty()) {
    std::cout << message;
    return;
  }
  printTaskTableHeader();
  for (const auto &info : tasks) {
    printTaskRow(info);
  }
}

void ConsoleHandler::printAllStatuses() const {
  printStatusTable(scheduler.getAllTasksStatus(), "No tasks in the system.");
}

void ConsoleHandler::printStatusesByStatus(const std::string &statusStr) const {
  auto parsed = EnumUtils::stringToTaskStatus(statusStr);
  if (!parsed) {
    std::cout << "Unknown status: " << statusStr << "\n";
    return;
  }
  printStatusTable(scheduler.getTasksByStatus(*parsed),
                   "No tasks with status " + statusStr + ".");
}

void ConsoleHandler::printStatusForTask(const std::string &taskName) const {
  auto status = scheduler.getTaskStatus(taskName);
  if (!status) {
    std::cout << "Task not found: " << taskName << "\n";
    return;
  }
  std::cout << taskName << ": " << EnumUtils::taskStatusToString(*status) << "\n";
}

void ConsoleHandler::processCommand(const std::string &command) {
  std::istringstream ss(command);
  std::string cmd;
  ss >> cmd;

  switch (EnumUtils::stringToCommand(cmd)) {
  case Command::QUIT:
    isRunning = false;
    std::cout << "Shutting down...\n";
    break;
  case Command::TASK: {
    std::string rest;
    std::getline(ss, rest);
    if (!rest.empty() && rest[0] == ' ')
      rest.erase(0, 1);
    if (rest.empty()) {
      std::cout << "Usage: task <filename> <type> <priority> [retries] [C/H] "
                   "[time...]\n";
      break;
    }
    if (!addTaskFromLine(rest)) {
      std::cout << "Failed to add task.\n";
    }
    break;
  }
  case Command::STATUS: {
    std::string arg;
    if (!(ss >> arg)) {
      printAllStatuses();
    } else if (arg == "-s") {
      std::string statusStr;
      ss >> statusStr;
      printStatusesByStatus(statusStr);
    } else {
      printStatusForTask(arg);
    }
    break;
  }
  case Command::CANCEL: {
    std::string name;
    if (ss >> name) {
      bool ok = scheduler.cancelTask(name);
      std::cout << (ok ? "Cancelled task: " + name + "\n"
                       : "Task not found: " + name + "\n");
    } else {
      std::cout << "Usage: cancel <name>\n";
    }
    break;
  }
  case Command::REPORT: {
    std::string name;
    if (ss >> name) {
      logger.exportReport(name, name + "_report.txt");
    } else {
      std::cout << "Usage: report <name>\n";
    }
    break;
  }
  default:
    std::cout << "Unknown command: " << cmd << "\n";
    break;
  }
}

bool ConsoleHandler::loadTasksFromFile() {
  std::string filepath;
  std::cout << "Provide filename/filepath please (txt): ";
  std::getline(std::cin, filepath);

  std::ifstream file(filepath);
  if (!file.is_open()) {
    std::cout << "Error: Could not open file: " << filepath << '\n';
    return false;
  }
  int successCount = addTasksFromStream(file);
  return successCount > 0;
}

bool ConsoleHandler::parseInput() {
  std::cout
      << "Input tasks:\n "
         "<filename> <type> <priority> [retries] [C/H] [time...]\n"
         "type: 1 - computational, 2 - file operations, 3 - data downloading\n"
         "Type d when done.\n";
  int successCount = addTasksFromStream(std::cin);
  return successCount > 0;
}

ConsoleHandler::ConsoleHandler(Scheduler &sched, TaskFactory &factory,
                               Logger &logger)
    : scheduler(sched), taskFactory(factory), logger(logger) {}

void ConsoleHandler::start() {
  isRunning = true;
  if (!getTasks()) {
    std::cout << "Shutting down...\n";
    isRunning = false;
    return;
  }

  std::string prompt = "\nType a command (status / status -s <status> / "
                       "status <name> / cancel name / report name / "
                       "task <filename> <type> <priority> [retries] "
                       "[C/H] [time...] / q):\n";
  std::cout<<prompt;

  std::string command;
  while (isRunning && std::getline(std::cin, command)) {
    if (command.empty())
      continue;
    processCommand(command);
    if (isRunning) {
      std::cout<<prompt;
    }
  }
};

int ConsoleHandler::addTasksFromStream(std::istream &input) {
  std::string line;
  int successCount = 0;

  while (std::getline(input, line)) {
    line.erase(0, line.find_first_not_of(" \t\r\n"));
    if (line.empty())
      continue;
    if (line == "d")
      break;

    if (addTaskFromLine(line)) {
      successCount++;
    }
  }
  return successCount;
}