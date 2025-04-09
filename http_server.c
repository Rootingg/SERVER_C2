#include <stdio.h>
#include "mongoose.h"

// Gestionnaire d'événements HTTP (signature avec 3 paramètres)
static void event_handler(struct mg_connection *conn, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        
        // Réponse simple pour tester que le serveur fonctionne
        mg_http_reply(conn, 200, "Content-Type: text/plain\r\n", "Serveur C2 actif\n");
    }
}

int main() {
    struct mg_mgr mgr;           // Gestionnaire d'événements Mongoose
    mg_mgr_init(&mgr);           // Initialisation du gestionnaire

    // Démarrer le serveur HTTP sur le port 8000
    mg_http_listen(&mgr, "http://0.0.0.0:8000", event_handler, NULL);
    printf("Serveur C2 démarré sur http://0.0.0.0:8000\n");

    // Boucle infinie pour traiter les événements
    for (;;) {
        mg_mgr_poll(&mgr, 1000); // Polling toutes les 1 seconde
    }

    mg_mgr_free(&mgr);           // Libération (jamais atteint dans cet exemple)
    return 0;
}