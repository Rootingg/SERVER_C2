#ifndef TASK_H
#define TASK_H

#include "c2_types.h"

extern struct Task *tasks;
extern pthread_mutex_t tasks_mutex;

struct Task* add_task(const char *agent_uid, const char *action);
struct Task* find_task(const char *agent_uid);
int update_task_result(const char *task_uid, const char *result);
void list_tasks();

#endif