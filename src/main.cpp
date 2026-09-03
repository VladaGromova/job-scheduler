#include "ConfigManager.hpp"
#include "ConsoleHandler.hpp"
#include "Executor.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
  ConfigManager configManager;
  std::string configFilename = (argc > 1) ? argv[1] : "config.txt";
  configManager.loadConfig(configFilename);
  std::cout << "\nMaxThreads: " << configManager.getMaxThreads()
            << "\nlogFile: " << configManager.getLogFileName()
            << "\ndefaultMaxRetries: " << configManager.getDefaultMaxRetries()
            << "\n\n";

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

  std::cout << "Program terminated.\n";
  return 0;
}