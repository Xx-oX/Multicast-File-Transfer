#include "multicast.h"

int main(int argc, char *argv[])
{
    /* create a datagram socket on which to receive */
    int sd;
    if((sd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        error("[ERROR]Failed to create socket (client side)!!!\n");
    }
    else{
        printf("Opening the datagram socket...OK\n");
    }

    /* Enable SO_REUSEADDR to allow multiple instances of this */
	/*  application to receive copies of the multicast datagrams. */
	
	int reuse = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
	{
		close(sd);
		error("[ERROR]Failed to set SO)REUSEADDR!!!\n");
	}
	else{
		printf("Setting SO_REUSEADDR...OK.\n");
    }

    /* Bind to the proper port number with the IP address */
	/*  specified as INADDR_ANY. */
    struct sockaddr_in localSock;
	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(PORT);
	localSock.sin_addr.s_addr = INADDR_ANY;
	if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock)))
	{
		close(sd);
		error("[ERROR]Failed to bind datagram socket!!!\n");
	}
	else{
		printf("Binding datagram socket...OK.\n");
    }

    /* Join the multicast group 226.1.1.1 on the local address*/
	/* interface. Note that this IP_ADD_MEMBERSHIP option must be */
	/* called for each local interface over which the multicast */
	/* datagrams are to be received. */
    struct ip_mreq group;

	group.imr_multiaddr.s_addr = inet_addr(GROUP_ADDR);
	/* your ip address */ 
	group.imr_interface.s_addr = inet_addr(LOCAL_ADDR); 
	/* IP_ADD_MEMBERSHIP:  Joins the multicast group specified */ 
	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
	{
		close(sd);
        error("[ERROR]Failed to add multicast group!!!\n");
	}
	else{
		printf("Adding multicast group...OK.\n");
    }

    /* receive file info */
    fileInfo_t file_info;
    if(recvfrom(sd, &file_info, sizeof(file_info), 0, NULL, NULL) < 0){
        close(sd);
        error("[ERROR]Failed to receive file info!!!\n");
    }
    else{
        printf("Reading file info...OK.\n");
    }

    char file_path[256];
    FILE *file_ptr;
    strncpy(file_path, file_info.path, sizeof(file_info.path));
    strcat(file_path, "_receive");
    if((file_ptr = fopen(file_path, "wb")) == NULL)
        error("[ERROR]Failed to open file!!!\n");


    /* receive file */
    char databuf[DATALEN];
    int ret;
    size_t total_size = 0;
    printf("Receiving file...");
    while(1){
        bzero(databuf, DATALEN);
        ret = recvfrom(sd, &databuf, sizeof(databuf), 0, NULL, NULL);
        if(ret < 0){
            close(sd);
            error("[ERROR]Failed to receive file!!!\n");
        }
        else if(strcmp(databuf, "XD") == 0){
            break;
        }
        else{
            fwrite(databuf, sizeof(char), ret, file_ptr);
            total_size += ret;
        }
    }
    printf("OK\n");

    printf("Received file size: %ldKb\n", total_size/1024);

    /* end of transfer */
    fclose(file_ptr);
    close(sd);

    return 0;
}