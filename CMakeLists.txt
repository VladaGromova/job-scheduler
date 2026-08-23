cmake_minimum_required(VERSION 3.15)
project(MojProjekt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(moj_program
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
)

target_include_directories(moj_program PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(moj_program PRIVATE /W4)
else()
    target_compile_options(moj_program PRIVATE -Wall -Wextra -Wpedantic)
endif()

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/hejo.txt"
        "${CMAKE_CURRENT_BINARY_DIR}/hejo.txt"
        COPYONLY
)