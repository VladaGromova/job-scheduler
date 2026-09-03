#include "../../include/tasks/Task.hpp"

Task::Task(std::string id, std::string fileName, int priority, int maxRetries,
           TaskMode mode,
           std::optional<std::chrono::time_point<std::chrono::system_clock>>
               requestedStart,
           std::optional<std::chrono::milliseconds> cycle)
    : id(std::move(id)), fileName(std::move(fileName)), priority(priority),
      maxRetries(maxRetries), status(TaskStatus::WAITING), mode(mode),
      createdAt(std::chrono::system_clock::now()) {
  if ((mode == TaskMode::SCHEDULED || mode == TaskMode::CYCLIC) &&
      !requestedStart) {
    throw std::invalid_argument(
        "SCHEDULED/CYCLIC task requires a requestedStartTime");
  }
  if (mode == TaskMode::CYCLIC && !cycle) {
    throw std::invalid_argument("CYCLIC task requires a cycleInterval");
  }
  if (requestedStart) {
    requestedStartTime = *requestedStart;
    nextExecutionTime = *requestedStart;
  }
  if (cycle) {
    cycleInterval = *cycle;
  }
}