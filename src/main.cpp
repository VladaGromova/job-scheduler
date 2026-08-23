#include "ConfigManager.hpp"
#include "ConsoleHandler.hpp"
#include "Executor.hpp"

#include <iostream>

int main() {
  ConfigManager configManager;
  std::string configFilename = "hejo.txt";
  configManager.loadConfig(configFilename);

  Logger logger(configManager.getLogFileName());
  Scheduler scheduler(logger);
  TaskFactory taskFactory(configManager.getDefaultMaxRetries());
  Executor executor(scheduler, logger, configManager.getMaxThreads());
  ConsoleHandler consoleHandler(scheduler, taskFactory, logger);

  scheduler.start();
  executor.start();

  consoleHandler.start();

  scheduler.stop();
  executor.stop();

  std::cout<<"Program terminated.\n";
  return 0;
}