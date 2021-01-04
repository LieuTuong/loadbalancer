CC = g++
OBJS = main.o loadbalancer.o socket.o log.o
OBJDIR = ../.obj/
LOGDIR = ../logs/
EXEFILE = run
MKDIR_P = mkdir -p 
.PHONY: all mkdirobj


all: mkdirobj $(OBJS)
	$(CC) $(OBJDIR)main.o  $(OBJDIR)log.o \
	$(OBJDIR)socket.o $(OBJDIR)loadbalancer.o \
	-o $(EXEFILE)

mkdirobj:
	$(MKDIR_P) $(OBJDIR) $(LOGDIR)

main.o: main.cpp socket.o loadbalancer.o
	$(CC) -c main.cpp -o $(OBJDIR)main.o

socket.o: socket.cpp
	$(CC) -c socket.cpp -o $(OBJDIR)socket.o

loadbalancer.o: loadbalancer.cpp 
	$(CC) -c loadbalancer.cpp -o $(OBJDIR)loadbalancer.o

log.o: log.cpp
	$(CC) -c log.cpp -o $(OBJDIR)log.o

clean:
	rm -rf $(OBJDIR) $(EXEFILE)