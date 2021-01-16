#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#define PORT 2001
#define LISTENQ 1024

//global constants
int list_s;
char *header200 = "HTTP/1.1 200 OK\r\nServer: CS241Serv v0.1\r\nContent-Type: text/html\r\n";
char *header200JPG = "HTTP/1.1 200 OK\r\nServer: CS241Serv v0.1\r\nContent-Type: image/jpeg\r\n\r\n";
char *header404 = "HTTP/1.1 404 Not Found\r\nServer: CS241Serv v0.1\r\nContent-Type: text/html\r\n";

typedef struct
{
    int returncode;
    char *filename;
} httpRequest;

typedef struct
{
    pthread_mutex_t mutexlock;
    int totalbytes;
} sharedVariables;

int lookup(const char *path)
{
    // Get file extension
    char *ext = (char *)strrchr(path, '.');

    if (strcasecmp(ext, ".html") == 0)
    {
        return 1;
    }
    else if (strcasecmp(ext, ".jpg") == 0)
    {
        return 0;
    }
}

// get a message from the socket until a blank line is recieved
char *getMessage(int fd)
{

    FILE *fp;
    if ((fp = fdopen(fd, "r")) == NULL)
    {
        fprintf(stderr, "Error opening file descriptor in getMessage()\n");
        exit(EXIT_FAILURE);
    }

    // Size variable for passing to getline
    size_t size = 1;

    char *block;

    // Allocate some memory for block and check it went ok
    if ((block = (char *)malloc(sizeof(char) * size)) == NULL)
    {
        fprintf(stderr, "Error allocating memory to block in getMessage\n");
        exit(EXIT_FAILURE);
    }

    // Set block to null
    *block = '\0';

    // Allocate some memory for tmp and check it went ok
    char *tmp;
    if ((tmp = (char *)malloc(sizeof(char) * size)) == NULL)
    {
        fprintf(stderr, "Error allocating memory to tmp in getMessage\n");
        exit(EXIT_FAILURE);
    }
    *tmp = '\0';

    // Int to keep track of what getline returns
    int end;
    // Int to help use resize block
    int oldsize = 1;

    // While getline is still getting data
    while ((end = getline(&tmp, &size, fp)) > 0)
    {
        // If the line its read is a caridge return and a new line were at the end of the header so break
        if (strcmp(tmp, "\r\n") == 0)
        {
            break;
        }

        // Resize block
        block = (char *)realloc(block, size + oldsize);
        // Set the value of oldsize to the current size of block
        oldsize += size;
        // Append the latest line we got to block
        strcat(block, tmp);
    }

    // Free tmp a we no longer need it
    free(tmp);

    // Return the header
    return block;
}

int sendMessage(int fd, char *msg)
{
    return send(fd, msg, strlen(msg), 0);
}

// Extracts the filename from GET request and adds directory public_html before it
char *getFileName(char *msg)
{
    char *file, x;

    // Allocate some memory for the filename and check it went OK
    if ((file = (char *)malloc(sizeof(char) * strlen(msg))) == NULL)
    {
        fprintf(stderr, "Error allocating memory to file in getFileName()\n");
        exit(EXIT_FAILURE);
    }

    sscanf(msg, "GET %s HTTP/1.1", file);
    printf("\n\n%s\n\n", msg);

    // Allocate some memory not in read only space to store "public_html"
    char *base;
    if ((base = (char *)malloc(sizeof(char) * (strlen(file) + 18))) == NULL)
    {
        fprintf(stderr, "Error allocating memory to base in getFileName()\n");
        exit(EXIT_FAILURE);
    }

    char *ph = "public_html";

    // Copy public_html to the non read only memory
    strcpy(base, ph);

    // Append the filename after public_html
    strcat(base, file);

    // Free file as we now have the file name in base
    free(file);

    // Return public_html/filetheywant.html
    return base;
}

// parse a HTTP request and return code 200 or 404
httpRequest parseRequest(char *msg)
{
    httpRequest ret;

    char *filename;
    if ((filename = (char *)malloc(sizeof(char) * strlen(msg))) == NULL)
    {
        fprintf(stderr, "Error allocating memory to filename in parseRequest()\n");
        exit(EXIT_FAILURE);
    }

    filename = getFileName(msg);

    // Check if it wants to use another directory
    char *badstring = "..";
    char *test = strstr(filename, badstring);

    // check if they asked for / and give them index.html
    int test2 = strcmp(filename, "public_html/");

    // Check if the page they want exists
    FILE *exists = fopen(filename, "r");

    // If the badstring is found in the filename
    if (test != NULL)
    {
        // Return a 404 header and 404.html
        ret.returncode = 404;
        ret.filename = "404.html";
    }

    // If they asked for / return index.html
    else if (test2 == 0)
    {
        ret.returncode = 200;
        ret.filename = "public_html/index.html";
    }

    // If they asked for a specific page and it exists because we opened it sucessfully return it
    else if (exists != NULL)
    {
        ret.returncode = 200;
        ret.filename = filename;
        // Close the file stream
        fclose(exists);
    }

    // If we get here the file they want doesn't exist so return a 404
    else
    {
        ret.returncode = 404;
        ret.filename = "404.html";
    }

    // Return the structure containing the details
    return ret;
}

// print a file out to a socket file descriptor
int printFile(int fd, char *filename)
{

    // if it is html file
    if (lookup(filename) == 1)
    {
        FILE *read;
        if ((read = fopen(filename, "r")) == NULL)
        {
            fprintf(stderr, "Error opening file in printFile()\n");
            exit(EXIT_FAILURE);
        }

        // Get the size of this file for printing out later on
        int totalsize;
        struct stat st;
        stat(filename, &st);
        totalsize = st.st_size;

        // Variable for getline to write the size of the line its currently printing to
        size_t size = 1;

        // Get some space to store each line of the file in temporarily
        char *temp;
        if ((temp = (char *)malloc(sizeof(char) * size)) == NULL)
        {
            fprintf(stderr, "Error allocating memory to temp in printFile()\n");
            exit(EXIT_FAILURE);
        }

        // Int to keep track of what getline returns
        int end;

        // While getline is still getting data
        while ((end = getline(&temp, &size, read)) > 0)
        {
            sendMessage(fd, temp);
        }
        //sendMessage(fd, "\n");
        free(temp);

        return totalsize;
    }
    else //send image
    {

        FILE *img = fopen(filename, "rb");
        fseek(img, 0, SEEK_END);
        unsigned long filesize = ftell(img);

        unsigned char *buffer = (unsigned char *)malloc(sizeof(unsigned char) * filesize);
        rewind(img);
        fread(buffer, sizeof(unsigned char), filesize, img);

        int sent = 0;
        while (sent < filesize)
        {
            int n = send(fd, buffer + sent, filesize - sent, 0);
            if (n == -1)
                break;
            sent += n;
        }

        sendMessage(fd, "\n");
        fclose(img);
    }
}

// clean up listening socket on ctrl-c
void cleanup(int sig)
{
    printf("Cleaning up connections and exiting.\n");
    if (close(list_s) < 0)
    {
        fprintf(stderr, "Error calling close()\n");
        exit(EXIT_FAILURE);
    }

    // Close the shared memory we used
    shm_unlink("/sharedmem");
    exit(EXIT_SUCCESS);
}

int printHeader(int fd, int returncode, char *filename)
{
    if (returncode == 200)
    {
        int type = lookup(filename);
        if (type == 1)
        {

            sendMessage(fd, header200);
            return strlen(header200);
        }
        else
        {
            sendMessage(fd, header200JPG);
            return strlen(header200JPG);
        }
    }
    else
    {
        sendMessage(fd, header404);
        return strlen(header404);
    }
}

// Increment the global count of data sent out
int recordTotalBytes(int bytes_sent, sharedVariables *mempointer)
{

    pthread_mutex_lock(&(*mempointer).mutexlock);
    (*mempointer).totalbytes += bytes_sent;
    pthread_mutex_unlock(&(*mempointer).mutexlock);

    return (*mempointer).totalbytes;
}

int main(int argc, char *argv[])
{
    int conn_s;                  //  connection socket
    short int port = PORT;       //  port number
    struct sockaddr_in servaddr; //  socket address structure

    // set up signal handler for ctrl-c
    (void)signal(SIGINT, cleanup);

    if ((list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "Error creating listening socket.\n");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(list_s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        fprintf(stderr, "Error calling bind()\n");
        exit(EXIT_FAILURE);
    }

    if ((listen(list_s, 10)) == -1)
    {
        fprintf(stderr, "Error Listening\n");
        exit(EXIT_FAILURE);
    }

    // Set up some shared memory to store our shared variables in
    // Close the shared memory we use just to be safe
    shm_unlink("/sharedmem");

    int sharedmem;
    if ((sharedmem = shm_open("/sharedmem", O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1)
    {
        fprintf(stderr, "Error opening sharedmem in main() errno is: %s ", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory to the size of my structure
    ftruncate(sharedmem, sizeof(sharedVariables));

    // Map the shared memory into our address space
    sharedVariables *mempointer;

    // Set mempointer to point at the shared memory
    mempointer = (sharedVariables *)mmap(NULL, sizeof(sharedVariables), PROT_READ | PROT_WRITE, MAP_SHARED, sharedmem, 0);

    if (mempointer == MAP_FAILED)
    {
        fprintf(stderr, "Error setting shared memory for sharedVariables in recordTotalBytes() error is %d \n ", errno);
        exit(EXIT_FAILURE);
    }

    // Initalise the mutex
    pthread_mutex_init(&(*mempointer).mutexlock, NULL);
    (*mempointer).totalbytes = 0; // Set total bytes sent to 0

    socklen_t addr_size = sizeof(servaddr);
    int headersize;
    int pagesize;
    int totaldata;
    int children = 0; //number of child process
    pid_t pid;

    // Loop infinitly serving requests
    while (1)
    {
        // If we haven't already spawned 10 children fork
        if (children <= 10)
        {
            pid = fork();
            children++;
        }

        if (pid == -1)
        {
            fprintf(stderr, "can't fork, error %d\n", errno);
            exit(1);
        }

        if (pid == 0)
        {
            // Have the child loop infinetly dealing with a connection then getting the next one in the queue
            while (1)
            {
                conn_s = accept(list_s, (struct sockaddr *)&servaddr, &addr_size);

                if (conn_s == -1)
                {
                    fprintf(stderr, "Error accepting connection \n");
                    exit(1);
                }

                // Get the message from the file descriptor
                char *header = getMessage(conn_s);

                // Parse the request
                httpRequest details = parseRequest(header);

                // Free header now were done with it
                free(header);

                // Print out the correct header
                headersize = printHeader(conn_s, details.returncode, details.filename);

                // Print out the file they wanted
                pagesize = printFile(conn_s, details.filename);

                // Increment our count of total datasent by all processes and get back the new total
                totaldata = recordTotalBytes(headersize + pagesize, mempointer);

                // Print out which process handled the request and how much data was sent
                printf("Process %d served a request of %d bytes. Total bytes sent %d  \n", getpid(), headersize + pagesize, totaldata);

                // Close the connection now were done
                close(conn_s);
            }
        }
    }

    return EXIT_SUCCESS;
}
