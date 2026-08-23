#include "ConfigManager.hpp"

#include <fstream>
#include <iostream>

bool ConfigManager::loadConfig(const std::string &configFilename) {
  if (configFilename.empty())
    return true;
  std::ifstream configFile(configFilename);

  if (!configFile.is_open()) {
    std::cerr << "[ConfigManager] Warning: Could not open config file: "
              << configFilename << ". Using default values.\n";
    return false;
  }

  int threads, retries;

  if (std::string logFile; configFile >> threads >> logFile >> retries) {
    if (threads > 0)
      maxThreads = threads;
    if (!logFile.empty())
      logFileName = logFile;
    if (retries >= 0)
      defaultMaxRetries = retries;

    std::cout << "[ConfigManager] Configuration loaded successfully from "
              << configFilename << "\n";
    return true;
  } else {
    std::cerr << "[ConfigManager] Error: Invalid config file format. Using "
                 "default values.\n";
    return false;
  }
}