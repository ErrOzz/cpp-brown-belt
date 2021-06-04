// #define FOR_SEND
#ifndef FOR_SEND
#include "test_runner.h"
#endif


#include <iostream>
#include <map>
#include <unordered_map>
#include <tuple>
#include <utility>


using namespace std;

#ifndef FOR_SEND
// Перечислимый тип для статуса задачи
enum class TaskStatus {
  NEW,          // новая
  IN_PROGRESS,  // в разработке
  TESTING,      // на тестировании
  DONE          // завершена
};

// Объявляем тип-синоним для map<TaskStatus, int>,
// позволяющего хранить количество задач каждого статуса
using TasksInfo = map<TaskStatus, int>;

#endif

class TeamTasks {
  unordered_map<string, TasksInfo> person_tasks;
public:
  // Получить статистику по статусам задач конкретного разработчика
  const TasksInfo& GetPersonTasksInfo(const string& person) const {
    return person_tasks.at(person);
  }

  // Добавить новую задачу (в статусе NEW) для конкретного разработчитка
  void AddNewTask(const string& person) {
    ++person_tasks[person][TaskStatus::NEW];
  }

  // Обновить статусы по данному количеству задач конкретного разработчика,
  // подробности см. ниже
  tuple<TasksInfo, TasksInfo> PerformPersonTasks(
      const string& person, int task_count) {
//    TasksInfo& tasks = person_tasks.at(person);
    TasksInfo& tasks = person_tasks[person];
    TasksInfo performed, rest;
    for (const auto& [status, count] : tasks) {
      auto shift_count = status == TaskStatus::DONE ? 0 : min(count, task_count);
      if (shift_count) {
        auto p_status = static_cast<TaskStatus>(static_cast<int>(status) + 1);
        performed[p_status] = shift_count;
      }
      if (auto untouched = count - shift_count; untouched) {
        rest[status] = untouched;
      }
      task_count -= shift_count;
    }
    TasksInfo result;
    for (int i = 0; i < 4; ++i) {
      TaskStatus status = static_cast<TaskStatus>(i);
      if (performed.count(status)) result[status] = performed[status];
      if (rest.count(status)) result[status] += rest[status];
    }
    swap(tasks, result);
    if (auto it = rest.find(TaskStatus::DONE); it != rest.end()) {
      rest.erase(it);
    }
    return make_tuple(move(performed), move(rest));
  }
};

// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь
void PrintTasksInfo(TasksInfo tasks_info) {
  cout << tasks_info[TaskStatus::NEW] << " new tasks" <<
      ", " << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress" <<
      ", " << tasks_info[TaskStatus::TESTING] << " tasks are being tested" <<
      ", " << tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
}

#ifndef FOR_SEND

void TestEmpty() {
  TeamTasks tasks;
  TasksInfo updated_tasks, untouched_tasks;
  tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Oz", 5);
  ASSERT(updated_tasks.size() == 0);
  ASSERT(untouched_tasks.size() == 0);
  ASSERT(tasks.GetPersonTasksInfo("Oz").size() == 0);
}

void TestSimple() {
  TeamTasks tasks;
  for (int i = 0; i < 10; ++i) {
    tasks.AddNewTask("Oz");
  }
  ASSERT(tasks.GetPersonTasksInfo("Oz").at(TaskStatus::NEW) == 10);
  TasksInfo updated_tasks, untouched_tasks;
  tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Oz", 5);
  ASSERT(untouched_tasks.at(TaskStatus::NEW) == 5);
  ASSERT(updated_tasks.at(TaskStatus::IN_PROGRESS) == 5);
  tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Oz", 20);
  ASSERT(updated_tasks.at(TaskStatus::IN_PROGRESS) == 5);
  ASSERT(updated_tasks.at(TaskStatus::TESTING) == 5);
  tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Oz", 7);
  ASSERT(updated_tasks.at(TaskStatus::TESTING) == 5);
  ASSERT(updated_tasks.at(TaskStatus::DONE) == 2);
  ASSERT(untouched_tasks.at(TaskStatus::TESTING) == 3);
  tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Oz", 100);
  ASSERT(updated_tasks.at(TaskStatus::DONE) == 8);
  ASSERT(tasks.GetPersonTasksInfo("Oz").at(TaskStatus::DONE) == 10);
}

#endif

int main() {

  #ifndef FOR_SEND

  TestRunner tr;
  RUN_TEST(tr, TestEmpty);
  RUN_TEST(tr, TestSimple);

  #endif

  TeamTasks tasks;
  tasks.AddNewTask("Ilia");
  for (int i = 0; i < 3; ++i) {
    tasks.AddNewTask("Ivan");
  }
  cout << "Ilia's tasks: ";
  PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"));
  cout << "Ivan's tasks: ";
  PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));

  TasksInfo updated_tasks, untouched_tasks;

  tie(updated_tasks, untouched_tasks) =
      tasks.PerformPersonTasks("Ivan", 2);
  cout << "Updated Ivan's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Ivan's tasks: ";
  PrintTasksInfo(untouched_tasks);

  tie(updated_tasks, untouched_tasks) =
      tasks.PerformPersonTasks("Ivan", 2);
  cout << "Updated Ivan's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Ivan's tasks: ";
  PrintTasksInfo(untouched_tasks);

  return 0;
}
