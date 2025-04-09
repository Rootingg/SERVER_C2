#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "agent.h"
#include "utils.h"

struct Agent *agents = NULL;
pthread_mutex_t agents_mutex = PTHREAD_MUTEX_INITIALIZER;

struct Agent* add_agent(const char *username, const char *hostname, const char *os) {
    char* uuid = generate_uuid();
    if (uuid == NULL) return NULL;
    
    struct Agent *new_agent = (struct Agent*)malloc(sizeof(struct Agent));
    if (new_agent == NULL) {
        free(uuid);
        return NULL;
    }
    
    strncpy(new_agent->uid, uuid, sizeof(new_agent->uid) - 1);
    new_agent->uid[sizeof(new_agent->uid) - 1] = '\0';
    
    strncpy(new_agent->username, username, sizeof(new_agent->username) - 1);
    new_agent->username[sizeof(new_agent->username) - 1] = '\0';
    
    strncpy(new_agent->hostname, hostname, sizeof(new_agent->hostname) - 1);
    new_agent->hostname[sizeof(new_agent->hostname) - 1] = '\0';
    
    strncpy(new_agent->os, os, sizeof(new_agent->os) - 1);
    new_agent->os[sizeof(new_agent->os) - 1] = '\0';
    
    new_agent->last_seen = time(NULL);
    new_agent->next = NULL;
    
    free(uuid);
    
    pthread_mutex_lock(&agents_mutex);
    
    if (agents == NULL) {
        agents = new_agent;
    } else {
        new_agent->next = agents;
        agents = new_agent;
    }
    
    pthread_mutex_unlock(&agents_mutex);
    
    return new_agent;
}

int update_agent_last_seen(const char *agent_uid) {
    int result = 0;
    
    pthread_mutex_lock(&agents_mutex);
    
    struct Agent *current = agents;
    while (current != NULL) {
        if (strcmp(current->uid, agent_uid) == 0) {
            current->last_seen = time(NULL);
            result = 1;
            break;
        }
        current = current->next;
    }
    
    pthread_mutex_unlock(&agents_mutex);
    
    return result;
}

struct Agent* find_agent(const char *agent_uid) {
    struct Agent *found = NULL;
    
    pthread_mutex_lock(&agents_mutex);
    
    struct Agent *current = agents;
    while (current != NULL) {
        if (strcmp(current->uid, agent_uid) == 0) {
            found = current;
            break;
        }
        current = current->next;
    }
    
    pthread_mutex_unlock(&agents_mutex);
    
    return found;
}

void list_agents() {
    pthread_mutex_lock(&agents_mutex);
    
    if (agents == NULL) {
        printf("Aucun agent connecté.\n");
    } else {
        printf("Liste des agents connectés :\n");
        printf("%-10s | %-32s | %-64s | %-32s | %-19s\n", 
               "UID", "Username", "Hostname", "OS", "Last Seen");
        printf("------------|----------------------------------|------------------------------------------------------------------|----------------------------------|-------------------\n");
        
        struct Agent *current = agents;
        while (current != NULL) {
            char time_str[20];
            struct tm *tm_info = localtime(&current->last_seen);
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
            
            printf("%-10s | %-32s | %-64s | %-32s | %s\n", 
                   current->uid, current->username, current->hostname, current->os, time_str);
            
            current = current->next;
        }
    }
    
    pthread_mutex_unlock(&agents_mutex);
}

int remove_agent(const char *agent_uid) {
    int result = 0;
    
    pthread_mutex_lock(&agents_mutex);
    
    struct Agent *current = agents;
    struct Agent *prev = NULL;
    
    while (current != NULL) {
        if (strcmp(current->uid, agent_uid) == 0) {
            if (prev == NULL) {
                agents = current->next;
            } else {
                prev->next = current->next;
            }
            
            free(current);
            result = 1;
            break;
        }
        
        prev = current;
        current = current->next;
    }
    
    pthread_mutex_unlock(&agents_mutex);
    
    return result;
}