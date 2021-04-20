#ifndef MULTICAST_H
#define MULTICAST_H

/* inclusions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* definitions */
#define GROUP_ADDR "226.1.1.1"
#define LOCAL_ADDR "127.0.0.1"
#define PORT 1234
#define DATALEN 1024

/* macros */
/*print error messages and end the process*/
#define error(msg) \
    { \
        perror(msg); \
        exit(EXIT_FAILURE); \
    } \

/* types */
typedef struct {
    char path[256];
    size_t size;
} fileInfo_t;

#endif
