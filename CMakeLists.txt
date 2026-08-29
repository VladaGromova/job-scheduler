cmake_minimum_required(VERSION 3.15)
project(JobScheduler VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(job_scheduler
        src/main.cpp
        src/tasks/Task.cpp
        include/tasks/Task.hpp
        include/enums/TaskStatus.hpp

        include/enums/TaskMode.hpp
        include/tasks/DownloadTask.hpp
        include/tasks/ComputeTask.hpp
        include/tasks/FileTask.hpp
        src/tasks/DownloadTask.cpp
        src/tasks/ComputeTask.cpp
        src/tasks/FileTask.cpp
        include/ConfigManager.hpp
        src/ConfigManager.cpp
        include/ConsoleHandler.hpp
        include/Scheduler.hpp
        include/TaskFactory.hpp
        include/LogMessage.hpp
        include/Logger.hpp
        include/Executor.hpp
        src/ConsoleHandler.cpp
        src/Scheduler.cpp
        src/TaskFactory.cpp
        src/Executor.cpp
        src/Logger.cpp
        include/helpers/DateTimeUtils.hpp
        include/helpers/TaskComparators.hpp
        include/helpers/EnumUtils.hpp
        include/enums/Command.hpp
)

target_include_directories(job_scheduler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(job_scheduler PRIVATE /W4)
else()
    target_compile_options(job_scheduler PRIVATE -Wall -Wextra -Wpedantic)
endif()
