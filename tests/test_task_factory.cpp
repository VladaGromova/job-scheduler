#include <catch2/catch_test_macros.hpp>
#include "TaskFactory.hpp"

TEST_CASE("TaskFactory creates ComputeTask for type 1", "[TaskFactory]") {
    TaskFactory factory(3);
    auto task = factory.createTaskFromLine("compute.exe 1 5");
    REQUIRE(task != nullptr);
    REQUIRE(task->taskTypeName() == "ComputeTask");
    REQUIRE(task->getPriority() == 5);
}

TEST_CASE("TaskFactory creates FileTask for type 2", "[TaskFactory]") {
    TaskFactory factory(3);
    auto task = factory.createTaskFromLine("file_copy.exe 2 8");
    REQUIRE(task != nullptr);
    REQUIRE(task->taskTypeName() == "FileTask");
}

TEST_CASE("TaskFactory creates DownloadTask for type 3", "[TaskFactory]") {
    TaskFactory factory(3);
    auto task = factory.createTaskFromLine("download.exe 3 2");
    REQUIRE(task != nullptr);
    REQUIRE(task->taskTypeName() == "DownloadTask");
}

TEST_CASE("TaskFactory rejects unknown task type", "[TaskFactory]") {
    TaskFactory factory(3);
    auto task = factory.createTaskFromLine("bad.exe 9 5");
    REQUIRE(task == nullptr);
}

TEST_CASE("TaskFactory uses default maxRetries when not specified", "[TaskFactory]") {
    TaskFactory factory(4);
    auto task = factory.createTaskFromLine("file.exe 2 5");
    REQUIRE(task != nullptr);
    REQUIRE(task->getMaxRetries() == 4);
}

TEST_CASE("TaskFactory parses maxRetries correctly", "[TaskFactory]") {
    TaskFactory factory(4);
    auto task = factory.createTaskFromLine("file.exe 2 5 10");
    REQUIRE(task != nullptr);
    REQUIRE(task->getMaxRetries() == 10);
}

TEST_CASE("TaskFactory parses SCHEDULED (H) mode correctly", "[TaskFactory]") {
    TaskFactory factory(3);
    auto task = factory.createTaskFromLine("invoice.exe 2 8 2 H 2027-01-01_12:00");
    REQUIRE(task != nullptr);
    REQUIRE(task->getMode() == TaskMode::SCHEDULED);
}

TEST_CASE("TaskFactory parses CYCLIC (C) mode with interval correctly", "[TaskFactory]") {
    TaskFactory factory(3);
    auto task = factory.createTaskFromLine("track.exe 2 4 3 C 2027-01-01_00:00 60000");
    REQUIRE(task != nullptr);
    REQUIRE(task->getMode() == TaskMode::CYCLIC);
    REQUIRE(task->getCycleInterval().count() == 60000);
}

TEST_CASE("TaskFactory generates unique id for each task", "[TaskFactory]") {
    TaskFactory factory(3);
    auto task1 = factory.createTaskFromLine("a.exe 1 5");
    auto task2 = factory.createTaskFromLine("b.exe 1 5");
    REQUIRE(task1->getId() != task2->getId());
}