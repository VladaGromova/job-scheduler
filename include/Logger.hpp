#pragma once
#include "LogMessage.hpp"

#include <fstream>

/**
 * @brief Responsible for writing logs to a file and generating task reports.
 *
 * All operations are protected by a mutex (mutable logMutex), allowing
 * log() to be safely called concurrently from multiple worker threads.
 */
class Logger {
private:
  std::ofstream logFile;
  mutable std::mutex logMutex;
  std::string logPath;

public:
  explicit Logger(const std::string &path);
  ~Logger();

  /// @brief Appends a single entry to the log file (thread-safe).
  void log(const LogMessage &message);

  /**
     * @brief Filters logs relating to a single task and writes them to a separate file.
     * @param taskId Task id for the report.
     * @param outputPath Path to output file.
     */
  void exportReport(const std::string &taskId,
                    const std::string &outputPath) const;
};