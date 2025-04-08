# Répartition des tâches pour le développement du serveur C2

## Objectifs globaux du projet
- Implémenter un serveur C2 en C avec **mongoose** pour gérer les requêtes HTTP des agents.
- Stocker les agents et les tâches en mémoire (listes chaînées).
- Fournir une interface CLI pour interagir avec les agents (lister, ajouter des tâches, voir les résultats).
- Utiliser des threads pour gérer le serveur HTTP et la CLI séparément.

---

## Développeur 1 (D1) : Gestion des structures de données et logique métier
**Rôle principal** : Définir les structures en mémoire, implémenter les fonctions de gestion des agents et des tâches, et assurer la synchronisation des accès.

### 1. Définition des structures de données
- **Objectif** : Créer les structures `Agent` et `Task` pour stocker les informations en mémoire.
- **Détails** :
  - `struct Agent` : Contient `uid` (37 octets pour UUID), `username` (32), `hostname` (64), `os` (32), `last_seen` (time_t), et un pointeur `next`.
  - `struct Task` : Contient `task_uid` (37), `agent_uid` (37), `action` (chaîne JSON), `result` (chaîne ou NULL), `completed` (int), et un pointeur `next`.
- **Complexité** : Faible (20 min).
- **Dépendances** : Aucune.

### 2. Fonction de génération d’UUID
- **Objectif** : Implémenter une fonction `generate_uuid()` pour générer des identifiants uniques.
- **Détails** :
  - Utiliser `rand()` pour générer un UUID simplifié (ex. : `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx`).
  - Retourner une chaîne allouée dynamiquement.
- **Complexité** : Faible (15 min).
- **Dépendances** : Aucune.

### 3. Gestion des agents
- **Objectif** : Implémenter les fonctions pour ajouter et mettre à jour les agents.
- **Détails** :
  - `add_agent(const char *username, const char *hostname, const char *os)` : Ajoute un nouvel agent à la liste chaînée avec un UID unique.
  - `update_agent_last_seen(const char *agent_uid)` : Met à jour le `last_seen` d’un agent existant.
  - Utiliser `pthread_mutex_lock/unlock` pour protéger l’accès à la liste globale `agents`.
- **Complexité** : Moyenne (1h).
- **Dépendances** : Structures définies (Tâche 1), UUID (Tâche 2).

### 4. Gestion des tâches
- **Objectif** : Implémenter les fonctions pour ajouter, chercher et mettre à jour les tâches.
- **Détails** :
  - `add_task(const char *agent_uid, const char *action)` : Ajoute une tâche à la liste chaînée avec un `task_uid` unique.
  - `find_task(const char *agent_uid)` : Retourne la première tâche non complétée pour un agent donné (ou NULL).
  - `update_task_result(const char *task_uid, const char *result)` : Met à jour le résultat d’une tâche et marque `completed = 1`.
  - Utiliser un mutex pour protéger l’accès à la liste globale `tasks`.
- **Complexité** : Moyenne (1h30).
- **Dépendances** : Structures définies (Tâche 1), UUID (Tâche 2).

**Temps total estimé pour D1** : ~3h05

---

## Développeur 2 (D2) : Serveur HTTP et interface CLI
**Rôle principal** : Configurer le serveur HTTP avec Mongoose, gérer les requêtes des agents, et implémenter l’interface CLI interactive.

### 1. Configuration du serveur HTTP avec Mongoose
- **Objectif** : Mettre en place le serveur HTTP pour écouter sur le port 8000.
- **Détails** :
  - Initialiser `mg_mgr` et appeler `mg_http_listen` sur `http://0.0.0.0:8000`.
  - Définir un gestionnaire d’événements `event_handler` pour traiter les requêtes HTTP.
  - Lancer une boucle infinie avec `mg_mgr_poll`.
- **Complexité** : Faible (30 min).
- **Dépendances** : Aucune (nécessite `mongoose.h` et `mongoose.c` dans le projet).

### 2. Traitement des requêtes HTTP
- **Objectif** : Implémenter la logique pour répondre aux requêtes `DECLARE`, `FETCH`, et `RESULT`.
- **Détails** :
  - Créer une fonction `handle_request(void *arg)` appelée dans un thread pour chaque requête.
  - **DECLARE** : Parser le JSON, appeler `add_agent`, et renvoyer `{"OK": {"UID": "<uid>"}}`.
  - **FETCH** : Parser l’UID, appeler `update_agent_last_seen` et `find_task`, renvoyer la tâche ou `{"error": "NotFound"}`.
  - **RESULT** : Parser les données, appeler `update_agent_last_seen` et `update_task_result`, renvoyer `{"status": "OK"}`.
  - Utiliser `mg_http_reply` pour envoyer les réponses.
- **Complexité** : Élevée (2h).
- **Dépendances** : Fonctions de gestion des agents et tâches (D1 : Tâches 3 et 4).

### 3. Interface CLI interactive
- **Objectif** : Implémenter une interface CLI dans un thread séparé pour interagir avec les agents.
- **Détails** :
  - Créer une fonction `cli_thread(void *arg)` qui tourne dans un thread détaché.
  - Commandes :
    - `list` : Lister tous les agents avec leurs détails (UID, username, hostname, os, last_seen).
    - `task <agent_uid> <action_json>` : Ajouter une tâche pour un agent donné.
    - `results` : Lister toutes les tâches avec leurs résultats.
    - `exit` : Terminer le programme.
  - Utiliser `fgets` pour lire les entrées utilisateur et parser les commandes.
- **Complexité** : Moyenne (1h30).
- **Dépendances** : Structures et fonctions de gestion (D1 : Tâches 1, 3, 4).

**Temps total estimé pour D2** : ~4h

---

## Plan de travail

### Jour 1
- **D1** :
  - Tâche 1 : Définir les structures (20 min).
  - Tâche 2 : Implémenter `generate_uuid` (15 min).
  - Tâche 3 : Implémenter la gestion des agents (1h).
- **D2** :
  - Tâche 1 : Configurer Mongoose (30 min).
  - Commencer Tâche 2 : Ébaucher `handle_request` sans dépendances complètes (1h).

### Jour 2
- **D1** :
  - Tâche 4 : Implémenter la gestion des tâches (1h30).
- **D2** :
  - Tâche 2 : Finaliser `handle_request` avec les fonctions de D1 (1h restant).
  - Tâche 3 : Implémenter la CLI (1h30).

### Jour 3 (optionnel)
- **D1 et D2** :
  - Tests conjoints : Compiler et tester le serveur avec des agents (1h).
  - Corrections de bugs et ajustements (30 min).

---
