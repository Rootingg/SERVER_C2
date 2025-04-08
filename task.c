#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "task.h"
#include "utils.h"

struct Task *tasks = NULL;
pthread_mutex_t tasks_mutex = PTHREAD_MUTEX_INITIALIZER;

struct Task* add_task(const char *agent_uid, const char *action) {
    char* uuid = generate_uuid();
    if (uuid == NULL) return NULL;
    
    struct Task *new_task = (struct Task*)malloc(sizeof(struct Task));
    if (new_task == NULL) {
        free(uuid);
        return NULL;
    }
    
    strncpy(new_task->task_uid, uuid, sizeof(new_task->task_uid) - 1);
    new_task->task_uid[sizeof(new_task->task_uid) - 1] = '\0';
    
    strncpy(new_task->agent_uid, agent_uid, sizeof(new_task->agent_uid) - 1);
    new_task->agent_uid[sizeof(new_task->agent_uid) - 1] = '\0';
    
    new_task->action = strdup(action);
    if (new_task->action == NULL) {
        free(uuid);
        free(new_task);
        return NULL;
    }
    
    new_task->result = NULL;
    new_task->completed = 0;
    new_task->next = NULL;
    
    free(uuid);
    
    pthread_mutex_lock(&tasks_mutex);
    
    if (tasks == NULL) {
        tasks = new_task;
    } else {
        new_task->next = tasks;
        tasks = new_task;
    }
    
    pthread_mutex_unlock(&tasks_mutex);
    
    return new_task;
}

struct Task* find_task(const char *agent_uid) {
    struct Task *found = NULL;
    
    pthread_mutex_lock(&tasks_mutex);
    
    struct Task *current = tasks;
    while (current != NULL) {
        if (strcmp(current->agent_uid, agent_uid) == 0 && current->completed == 0) {
            found = current;
            break;
        }
        current = current->next;
    }
    
    pthread_mutex_unlock(&tasks_mutex);
    
    return found;
}

int update_task_result(const char *task_uid, const char *result) {
    int success = 0;
    
    pthread_mutex_lock(&tasks_mutex);
    
    struct Task *current = tasks;
    while (current != NULL) {
        if (strcmp(current->task_uid, task_uid) == 0) {
            if (current->result != NULL) {
                free(current->result);
            }
            
            current->result = strdup(result);
            if (current->result != NULL) {
                current->completed = 1;
                success = 1;
            }
            break;
        }
        current = current->next;
    }
    
    pthread_mutex_unlock(&tasks_mutex);
    
    return success;
}

void list_tasks() {
    pthread_mutex_lock(&tasks_mutex);
    
    if (tasks == NULL) {
        printf("Aucune tâche.\n");
    } else {
        printf("Liste des tâches :\n");
        printf("%-10s | %-10s | %-30s | %-10s\n", 
               "Task UID", "Agent UID", "Action", "Completed");
        printf("------------|------------|--------------------------------|------------\n");
        
        struct Task *current = tasks;
        while (current != NULL) {
            printf("%-10s | %-10s | %-30s | %-10s\n", 
                   current->task_uid, 
                   current->agent_uid, 
                   current->action, 
                   current->completed ? "Oui" : "Non");
            
            current = current->next;
        }
    }
    
    pthread_mutex_unlock(&tasks_mutex);
}