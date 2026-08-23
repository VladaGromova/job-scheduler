#pragma once

enum class TaskStatus {
  SCHEDULED,
  WAITING,
  RUNNING,
  FINISHED,
  FAILED,
  CANCELLED
};