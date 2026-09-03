#pragma once
#include "tasks/Task.hpp"

/**
 * @brief Builds concrete Task objects from a command.
 *
 * Maps a numeric task-type code (1/2/3) to a concrete class
 * (ComputeTask/FileTask/DownloadTask).
 * Generates unique task ids in the form "number_filename".
 */
class TaskFactory {
private:
  int defaultMaxRetries;
  std::atomic<int> taskId{1};
  std::string generateTaskId(const std::string &fileName);

public:
  explicit TaskFactory(int defaultMaxRetries);

  /**
     * @brief Parses a line of text in the format
     *        "<file> <type> <priority> [retries] [C/H] [time...]" and builds a task.
     * @return The created task, or nullptr if the format/values are invalid.
     */
  std::unique_ptr<Task> createTaskFromLine(const std::string &line);
};