#ifndef C2_TYPES_H
#define C2_TYPES_H

#include <time.h>

struct Agent {
    char uid[38];
    char username[33];
    char hostname[65];
    char os[33];
    time_t last_seen;
    struct Agent *next;
};

struct Task {
    char task_uid[38];
    char agent_uid[38];
    char *action;
    char *result;
    int completed;
    struct Task *next;
};

#endif