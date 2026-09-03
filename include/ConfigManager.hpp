#pragma once
#include <string>

class ConfigManager {
private:
  int maxThreads{1};
  std::string logFileName{"default_logs.txt"};
  int defaultMaxRetries{1};

public:
  ConfigManager() = default;
  bool loadConfig(const std::string &configFilename);
  [[nodiscard]] int getMaxThreads() const { return maxThreads; }
  [[nodiscard]] int getDefaultMaxRetries() const { return defaultMaxRetries; }
  [[nodiscard]] const std::string &getLogFileName() const {
    return logFileName;
  }
};