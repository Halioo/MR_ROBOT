#
# Hello Robot C - Makefile des sources du projet.
#
# @author Matthias Brun, Clément Puybareau
#

PACKAGES = robocom watchdog mailbox liste_chainee

# Inclusion depuis le niveau du package.
CCFLAGS += -I.

# Compilation.
all:
	@for p in $(PACKAGES); do (cd $$p; $(MAKE) $@); done

clean:
	@for p in $(PACKAGES); do (cd $$p; $(MAKE) $@); done
