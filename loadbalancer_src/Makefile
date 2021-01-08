CC = g++
OBJS = main.o loadbalancer.o socket.o log.o config.o
OBJDIR = ../.obj/
LOGDIR = ../loadbalancer_logs/
EXEFILE = run
MKDIR_P = mkdir -p 
.PHONY: all mkdirobj


all: mkdirobj $(OBJS)
	$(CC) $(OBJDIR)main.o $(OBJDIR)log.o $(OBJDIR)config.o \
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

config.o: config.cpp
	$(CC) -c config.cpp -o $(OBJDIR)config.o
	
clean:
	rm -rf $(OBJDIR) $(EXEFILE)