#ifndef AGENT_H
#define AGENT_H

#include "c2_types.h"

extern struct Agent *agents;
extern pthread_mutex_t agents_mutex;

struct Agent* add_agent(const char *username, const char *hostname, const char *os);
int update_agent_last_seen(const char *agent_uid);
struct Agent* find_agent(const char *agent_uid);
void list_agents();
int remove_agent(const char *agent_uid);

#endif