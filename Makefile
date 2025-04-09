# Nom de l'exécutable
TARGET = c2_server

# Compilateur à utiliser
CC = gcc

# Pas d'options supplémentaires
CFLAGS =

# Bibliothèques à lier
LIBS = -ljson-c -pthread

# Liste des fichiers sources
SOURCES = http_server.c agent.c task.c cli.c utils.c mongoose.c

# Liste des fichiers objets générés à partir des sources
OBJECTS = $(SOURCES:.c=.o)

# Règle par défaut : construire l'exécutable
all: $(TARGET)

# Règle pour lier les objets et créer l'exécutable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

# Règle pour compiler chaque fichier source en fichier objet
%.o: %.c
	$(CC) -c $< -o $@

# Nettoyage des fichiers générés
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all clean