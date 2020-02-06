#
# Hello Robot C - Makefile des sources du projet.
#
# @author Matthias Brun
#

#
# Organisation des sources.
#

# Packages du projet (à compléter si besoin est).
PACKAGES = commando telco

# Un niveau de package est accessible.
SRC_SERVER = $(wildcard */*.c)
SRC_CLIENT = $(wildcard */*.c)

# Pour ajouter un second niveau :		
# SRC += $(wildcard */*/*.c)

OBJ_SERVER = $(SRC_SERVER:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)

# Point d'entrée du programme.
MAIN_SERVER = main_server.c
MAIN_CLIENT = main_client.c

# Gestion automatique des dépendances.
DEP_SERVER = $(MAIN_SERVER:.c=.d)
DEP_CLIENT = $(MAIN_CLIENT:.c=.d)

# Exécutable à générer.
#EXEC = ../$(PROG)
EXEC_SERVER = ../$(BINDIR)/robot_pc1
EXEC_CLIENT = ../$(BINDIR)/robot_pc2


# Inclusion depuis le niveau du package.
CCFLAGS += -I.

#
# Règles du Makefile.
#

# Compilation.
all:
	@for p in $(PACKAGES); do (cd $$p; $(MAKE) $@); done
	@$(MAKE) CCFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)" $(EXEC_SERVER)
	@$(MAKE) CCFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)" $(EXEC_CLIENT)

$(EXEC_SERVER): $(OBJ) $(MAIN)
	$(CC) $(CCFLAGS) $(OBJ_SERVER) $(MAIN_SERVER) -MF $(DEP_SERVER) -o $(EXEC_SERVER) $(LDFLAGS)

$(EXEC_CLIENT): $(OBJ) $(MAIN)
	$(CC) $(CCFLAGS) $(OBJ_CLIENT) $(MAIN_CLIENT) -MF $(DEP_CLIENT) -o $(EXEC_CLIENT) $(LDFLAGS)

# Nettoyage.
.PHONY: clean

clean:
	@for p in $(PACKAGES); do (cd $$p; $(MAKE) $@); done
	@rm -f $(DEP_SERVER)
	@rm -f $(DEP_CLIENT)

-include $(DEP_SERVER)
-include $(DEP_CLIENT)

