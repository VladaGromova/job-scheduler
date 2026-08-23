#pragma once
#include "LogMessage.hpp"

#include <fstream>

class Logger {
private:
  std::ofstream logFile;
  mutable std::mutex logMutex;
  std::string logPath;

public:
  explicit Logger(const std::string& path);
  ~Logger();
  void log(const LogMessage& message);
  void exportReport (const std::string& taskId, const std::string& outputPath) const;
};