# ===== Makefile =====
CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -pthread

SRC_DIR = src
OBJ_DIR = obj

# Crée les dossiers si besoin
$(shell mkdir -p $(OBJ_DIR))

# Liste des fichiers
CLIENT_SRC = $(SRC_DIR)/sh13.c
SERVER_SRC = $(SRC_DIR)/server.c

CLIENT_OBJ = $(OBJ_DIR)/sh13.o
SERVER_OBJ = $(OBJ_DIR)/server.o

# Cibles principales
all: $(SRC_DIR)/sh13 $(SRC_DIR)/server

# Création de l'exécutable client dans src/
$(SRC_DIR)/sh13: $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Création de l'exécutable serveur dans src/
$(SRC_DIR)/server: $(SERVER_OBJ)
	$(CC) -o $@ $^ -pthread

# Compilation des fichiers .c vers .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(OBJ_DIR)/*.o $(SRC_DIR)/sh13 $(SRC_DIR)/server

.PHONY: all clean
