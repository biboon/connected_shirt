# Makefile pour generer le serveur 

OBJS = serveur.o http.o
LOCAL_LIBS  = -L ../Communication -L ../Threads
LOCAL_LIBS += -lcom -lthrd
CFLAGS     += -I ../Communication -I ../Threads -lpthread

# Cible generale
all: serveur

# La cible de nettoyage
clean: 
	rm -f core *.o serveur http

# Les cibles pour l'executable
serveur: $(OBJS)
	$(CC) $(CFLAGS) -o serveur $(OBJS) $(LOCAL_LIBS) $(LIBS)

serveur.o: serveur.c serveur.h
http.o: http.c http.h
