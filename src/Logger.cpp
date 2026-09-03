#include "Logger.hpp"

#include "helpers/DateTimeUtils.hpp"
#include "helpers/EnumUtils.hpp"

#include <iostream>

Logger::Logger(const std::string &path) : logPath(path) {
  logFile.open(logPath, std::ios::app); // overwrite?
  if (!logFile.is_open()) {
    throw std::runtime_error("Logger: could not open log file: " + logPath);
  }
}

Logger::~Logger() {
  if (logFile.is_open()) {
    logFile.close();
  }
}

void Logger::log(const LogMessage &message) {
  std::lock_guard<std::mutex> lock(logMutex);

  logFile << DateTimeUtils::formatDateTime(message.timestamp) << " | "
          << "[" << message.taskId << "] | "
          << EnumUtils::taskStatusToString(message.taskStatus)
          << " | "
          << message.message << std::endl;
}

void Logger::exportReport(const std::string& taskId,
                           const std::string& outputPath) const {
  std::lock_guard<std::mutex> lock(logMutex);

  std::ifstream in(logPath);
  if (!in.is_open()) {
    throw std::runtime_error("exportReport: could not open log file: " + logPath);
  }

  const std::string marker = "[" + taskId + "]";

  std::vector<std::string> matchedLines;
  std::string line;
  while (std::getline(in, line)) {
    if (line.find(marker) != std::string::npos) {
      matchedLines.push_back(line);
    }
  }

  if (matchedLines.empty()) {
    std::cout << "No log entries found for task: " << taskId << ". Report not created.\n";
    return;
  }

  std::ofstream out(outputPath);
  if (!out.is_open()) {
    throw std::runtime_error("exportReport: could not open output file: " + outputPath);
  }

  for (const auto& matchedLine : matchedLines) {
    out << matchedLine << "\n";
  }

  out << "\n--- Report generated for task: " << taskId << " (" << matchedLines.size() << " entries) ---\n";
  std::cout << "Report saved to " << outputPath << "_report.txt\n";
}