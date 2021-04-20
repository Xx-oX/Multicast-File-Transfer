#include "multicast.h"

int main(int argc, char *argv[])
{
    if(argc != 2) error("[ERROR]Wrong arguments!!!\n");

    /* preparing file for transfer */
    char file_path[256];
    struct stat file_stat;
    FILE *file_ptr;
    strcpy(file_path, argv[1]);
    if(lstat(file_path, &file_stat) < 0)
        error("[ERROR]Failed to get file status!!!\n");
    if((file_ptr = fopen(file_path, "rb")) == NULL)
        error("[ERROR]Failed to open file!!!\n");

    /* create a datagram socket */
    int sd;
    if((sd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        error("[ERROR]Failed to create socket (server side)!!!\n");
    }
    else{
        printf("Opening the datagram socket...OK\n");
    }

    /* initialize the group sockaddr structure with a 
        groupaddress of 226.1.1.1 & port 4321 */
    struct sockaddr_in groupSock;
    memset((char *) &groupSock, 0, sizeof(groupSock));
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr(GROUP_ADDR);
    groupSock.sin_port = htons(PORT);

    /* Set local interface for outbound multicast datagrams. */
	/* The IP address specified must be associated with a local, */
	/*  multicast capable interface. */
    struct in_addr localInterface;
    localInterface.s_addr = inet_addr(LOCAL_ADDR);

    /* IP_MULTICAST_IF:  Sets the interface over which outgoing multicast datagrams are sent. */
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0){
	  error("[ERROR]Failed to set local interface!!!\n");
	}
	else{
	  printf("Setting the local interface...OK\n");
    }

    /* send file info */
    fileInfo_t file_info = {
        .size = file_stat.st_size
    };
    strncpy(file_info.path, file_path, sizeof(file_path));

    if(sendto(sd, &file_info, sizeof(file_info), 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0){
		error("[ERROR]Failed to send file info!!!\n");
	}
	else{
	    printf("Sending file info...OK\n");
    }

    /* send file */
    int ret;
    char databuf[DATALEN];

    printf("Sending file...");
    while((ret = fread(databuf, sizeof(char), sizeof(databuf), file_ptr))){
        sendto(sd, databuf, ret, 0, (struct sockaddr *) &groupSock, sizeof(groupSock));
    }
    printf("OK\n");

    sendto(sd, "XD", sizeof("XD"), 0, (struct sockaddr *) &groupSock, sizeof(groupSock));

    printf("File size: %ldKb\n", file_info.size/1024);

    /* end of transfer */
    fclose(file_ptr);
    close(sd);

    return 0;
}