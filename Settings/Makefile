OBJS = setupShirt.o
LOCAL_LIBS  = -L ../Communication
LOCAL_LIBS += -lcom
CFLAGS     += -I ../Communication

# Cible generale
all: setupShirt

# La cible de nettoyage
clean:
	rm -f core *.o setupShirt

# Les cibles pour l'executable
setupShirt: $(OBJS)
	$(CC) $(CFLAGS) -o setupShirt $(OBJS) $(LOCAL_LIBS)

setupShirt.o: setupShirt.c
