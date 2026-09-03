#pragma once
#include <string>

/**
 * @brief Loads program settings from a configuration file.
 *
 * If the file is missing falls back to default values.
 */
class ConfigManager {
private:
  int maxThreads{1};
  std::string logFileName{"default_logs.txt"};
  int defaultMaxRetries{1};

public:
  ConfigManager() = default;

  /// @brief Loads configuration from a file; returns false if it is missing.
  bool loadConfig(const std::string &configFilename);
  [[nodiscard]] int getMaxThreads() const { return maxThreads; }
  [[nodiscard]] int getDefaultMaxRetries() const { return defaultMaxRetries; }
  [[nodiscard]] const std::string &getLogFileName() const {
    return logFileName;
  }
};