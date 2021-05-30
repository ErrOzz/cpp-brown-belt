#include <iostream>
#include <map>
#include <unordered_map>
#include <tuple>
#include <utility>

//#define FOR_SEND

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
    TasksInfo& tasks = person_tasks.at(person);
    TasksInfo performed, rest;
    for (auto& [status, count] : tasks) {
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
    return tie(performed, rest);
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

int main() {
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
