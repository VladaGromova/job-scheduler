#include "Logger.hpp"

#include "helpers/DateTimeUtils.hpp"
#include "helpers/EnumUtils.hpp"

Logger::Logger(const std::string& path) : logPath(path) {
  logFile.open(logPath, std::ios::app);   // overwrite?
  if (!logFile.is_open()) {
    throw std::runtime_error("Logger: could not open log file: " + logPath);
  }
}

Logger::~Logger() {
  if (logFile.is_open()) {
    logFile.close();
  }
}

void Logger::log(const LogMessage& message) {
  std::lock_guard<std::mutex> lock(logMutex);

  logFile << DateTimeUtils::formatDateTime(message.timestamp) << " | "
          << "[" << message.taskId << "] | "
          << taskStatusToString(message.taskStatus) << " | "
          // << "mode=" << taskModeToString(message.taskMode) << " | "
          // << "type=" << message.taskType << " | "
          << message.message
          << std::endl;   // endl celowo, żeby flushować od razu - ważne przy crashu/kill
}

void Logger::exportReport (const std::string& taskId, const std::string& outputPath) const {
  std::lock_guard<std::mutex> lock(logMutex);

  std::ifstream in(logPath);
  if (!in.is_open()) {
    throw std::runtime_error("exportReport: could not open log file: " + logPath);
  }

  std::ofstream out(outputPath);
  if (!out.is_open()) {
    throw std::runtime_error("exportReport: could not open output file: " + outputPath);
  }

  const std::string marker = "task=" + taskId + " |";

  std::string line;
  int matchCount = 0;
  while (std::getline(in, line)) {
    if (line.find(marker) != std::string::npos) {
      out << line << "\n";
      matchCount++;
    }
  }

  if (matchCount == 0) {
    out << "No log entries found for task: " << taskId << "\n";
  }

  out << "\n--- Report generated for task: " << taskId
      << " (" << matchCount << " entries) ---\n";
}