//Bryan Nguyen 1001719605
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h> //needed for socket related functions
#include <arpa/inet.h> //needed for internet related functions

int main(int argc, char* argv[])
{
    int port;
    if(argc > 2 && argc < 4)
    {
        port = atoi(argv[2]);
        char* ip = malloc(strlen(argv[2]) + 1);
        strcpy(ip, argv[2]);
    }
    else
    {
        int port = 0;
        char* ip = NULL;
    }

    int socket_desc; //status of socket creation
    int client_sock; //status of client connection
    int client_size;//
    struct sockaddr_in server_addr; //struct containing information about our server address
    struct sockaddr_in client_addr; //struct containing information about our client address
    char server_message[2000]; //array containing the server message
    char client_message[2000]; //array containing the clinet message

    //Set message arrays to null
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    // Create socket. will return a negative if unsucessful
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    //Check to see if socket sucessfully created
    if(socket_desc < 0){
    	//Display error and exit if not
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET; //set socket domain
    server_addr.sin_port = htons(8000); //port
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //*ip

    // Bind to the set port and IP
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0)
    {
    	//Display error and exit if unsucessful
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");
    // Listen for clients
    if(listen(socket_desc, 1) < 0){
        printf("Error while listening\n");
        return -1;
    }
    printf("\nListening for incoming connections.....\n");

    // Accept an incoming connection and get information about our client
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);

    if (client_sock < 0){
    	//Display error and don't connect if unable to connect
        printf("Can't accept\n");
        return -1;
    }
    printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    if (send(client_sock, server_message, strlen(server_message), 0) < 0){
        printf("Can't send\n");
        return -1;
    }
    while(1)
    {
        // Receive client's message:
        if (recv(client_sock, client_message, sizeof(client_message), 0) < 0){
            printf("Couldn't receive\n");
            return -1;
        }

        char hexsize[2];

        sprintf(hexsize, "\n%c", client_message[8]);

        int number = (int)strtol(hexsize, NULL, 16);

        int pSize = number+3;

        //checksum
        int sum = 0, i;
        for(i = 0; i < pSize*4 - 4; i+=5)
        {
            char checkSize[4];
            //printf("\n%c%c%c%c ", client_message[i], client_message[i+1], client_message[i+2], client_message[i+3]);
            sprintf(checkSize, "%c%c%c%c", client_message[i], client_message[i+1], client_message[i+2], client_message[i+3]);
            const char *hexstring = checkSize;
            int num = (int)strtol(hexstring, NULL, 0);
            sum ^= num;
        }
        //IF VALID
        char checkSum[4];
        sprintf(checkSum, "0x%02X", sum);
        //printf("\n-%s-", checkSum);

        char checkS[4];
        for(i = pSize*4 - 2; i < pSize*4; i+=5)
        {
            //printf("\n-%c%c%c%c- ", client_message[i], client_message[i+1], client_message[i+2], client_message[i+3]);
            sprintf(checkS, "%c%c%c%c", client_message[i], client_message[i+1], client_message[i+2], client_message[i+3]);

        }
        int flag = 1; // 1 if true 0 if false

        for(i = 0; i < 4; i++)
        {
            if(checkSum[i] != checkS[i]) //if they are not the same
            {
                flag = 0;
            }
        }
        if(flag == 1)
        {
            printf("\nMsg from client: %s\n", client_message);
            strcpy(server_message, "Message Received.");
        }
        else
        {
            strcpy(server_message, "Message Rejected.");
        }
        //RESPONSE
        if (send(client_sock, server_message, strlen(server_message), 0) < 0)
        {
            printf("Can't send\n");
            return -1;
        }

        break;
    }

    // Closing the socket:
    close(client_sock);
    close(socket_desc);

    return 0;
}
