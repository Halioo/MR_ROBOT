#
# Hello Robot C - Makefile du package robot des sources.
#
# @author Matthias Brun
#

#
# Organisation des sources.
#

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)

# Inclusion depuis le niveau du package.
CCFLAGS += -I..


# Exécutable à générer.
EXEC = ../../$(BINDIR)/robot_pc1
MAIN = main_server.c

#
# Règles du Makefile.
#

# Compilation.
all:
	@$(MAKE) CCFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)" $(EXEC)


$(EXEC): $(OBJ) $(MAIN)
	$(CC) $(CCFLAGS) $(OBJ) $(MAIN) -MF $(DEP) -o $(EXEC) $(LDFLAGS)

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@

# Nettoyage.
.PHONY: clean

clean:
	@rm -f $(OBJ) $(DEP)


-include $(DEP)

