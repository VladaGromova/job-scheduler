#include <catch2/catch_test_macros.hpp>
#include "Scheduler.hpp"
#include "TaskFactory.hpp"
#include "Logger.hpp"

namespace {
    Logger makeTestLogger() {
        return Logger("test_logs.txt");
    }
}

TEST_CASE("Scheduler addTask adds correct tasks", "[Scheduler]") {
    Logger logger = makeTestLogger();
    Scheduler scheduler(logger);
    TaskFactory factory(3);

    auto task = factory.createTaskFromLine("a.exe 1 5");
    std::string id = task->getId();
    scheduler.addTask(std::move(task));

    auto status = scheduler.getTaskStatus(id);
    REQUIRE(status.has_value());
    REQUIRE(*status == TaskStatus::WAITING);
}

TEST_CASE("Scheduler getTaskStatus returns nullopt for not existing id", "[Scheduler]") {
    Logger logger = makeTestLogger();
    Scheduler scheduler(logger);

    auto status = scheduler.getTaskStatus("does-not-exist");
    REQUIRE(status == std::nullopt);
}

TEST_CASE("Scheduler cancelTask marks task as CANCELLED", "[Scheduler]") {
    Logger logger = makeTestLogger();
    Scheduler scheduler(logger);
    TaskFactory factory(3);

    auto task = factory.createTaskFromLine("a.exe 1 5");
    std::string id = task->getId();
    scheduler.addTask(std::move(task));

    bool cancelled = scheduler.cancelTask(id);
    REQUIRE(cancelled);

    auto status = scheduler.getTaskStatus(id);
    REQUIRE(*status == TaskStatus::CANCELLED);
}

TEST_CASE("Scheduler cancelTask returns false for not existing id", "[Scheduler]") {
    Logger logger = makeTestLogger();
    Scheduler scheduler(logger);

    REQUIRE_FALSE(scheduler.cancelTask("does-not-exist"));
}

TEST_CASE("Scheduler getAllTasksStatus returns all added tasks", "[Scheduler]") {
    Logger logger = makeTestLogger();
    Scheduler scheduler(logger);
    TaskFactory factory(3);

    scheduler.addTask(factory.createTaskFromLine("a.exe 1 5"));
    scheduler.addTask(factory.createTaskFromLine("b.exe 2 3"));
    scheduler.addTask(factory.createTaskFromLine("c.exe 3 8"));

    auto all = scheduler.getAllTasksStatus();
    REQUIRE(all.size() == 3);
}

TEST_CASE("Scheduler getTasksByStatus returns correct filtered tasks", "[Scheduler]") {
    Logger logger = makeTestLogger();
    Scheduler scheduler(logger);
    TaskFactory factory(3);

    auto task1 = factory.createTaskFromLine("a.exe 1 5");
    std::string id1 = task1->getId();
    scheduler.addTask(std::move(task1));

    scheduler.addTask(factory.createTaskFromLine("b.exe 1 3"));

    scheduler.cancelTask(id1);

    auto cancelled = scheduler.getTasksByStatus(TaskStatus::CANCELLED);
    REQUIRE(cancelled.size() == 1);
    REQUIRE(cancelled[0].id == id1);

    auto waiting = scheduler.getTasksByStatus(TaskStatus::WAITING);
    REQUIRE(waiting.size() == 1);
}

TEST_CASE("Scheduler marks task as FAILED with time in the past", "[Scheduler]") {
    Logger logger = makeTestLogger();
    Scheduler scheduler(logger);
    TaskFactory factory(3);

    auto task = factory.createTaskFromLine("old.exe 2 5 1 H 2020-01-01_00:00");
    REQUIRE(task != nullptr);
    std::string id = task->getId();

    scheduler.addTask(std::move(task));

    auto status = scheduler.getTaskStatus(id);
    REQUIRE(status.has_value());
    REQUIRE(*status == TaskStatus::FAILED);
}