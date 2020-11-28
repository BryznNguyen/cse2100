#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> //needed for socket related functions
#include <arpa/inet.h> //needed for internet related functions

int main(int argc, char* argv[])
{
    int port;
    if(argc > 2 && argc < 4)
    {
        port = argv[2];
        char* ip = malloc(strlen(argv[2]) + 1);
        strcpy(ip, argv[2]);
    }
    else
    {
        int port = 0;
        char* ip = NULL;
    }

    int socket_desc;
    struct sockaddr_in server_addr;
    char server_message[2000];
    char client_message[2000];

    // Clean buffers:
    memset(server_message,'\0',sizeof(server_message));
    memset(client_message,'\0',sizeof(client_message));

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc < 0)
        {
            printf("Unable to create socket\n");
            return -1;
        }
    printf("Socket created successfully\n");

    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000); //port
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //*ip

    // Send connection request to server:
    if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Unable to connect\n");
        return -1;
    }
    printf("Connected with server successfully\n");

    // Get input from the user:
    while(1)
    {
        int length = 50; //lab 5 default
        int packetlen = 255; //lab 5 default
        char str[length];
        printf("Enter your string: ");
        scanf("%[^\n]", str);
        int packet[packetlen];
        packet[0] = 170; //AA start byte

        int size = 3;  //Default has 3
        int i;
        for(i = 0; i < str[i]; i++)
        {
            size++;
        }

        packet[1] = size;  //saves size + 3

        //assigns letters to int array by ascii value
        for(i = 0; i < str[i] ; i++)
        {
            packet[i+2] = str[i];  //assigning message part
        }

        //Calculating checksum
        int sum = 0;
        for(i = 0; i < size -1; i++)
        {
            sum ^= packet[i];  //XOR calculator
        }
        packet[size-1] = sum;

        //prints out the packet in hex
        int j;
        char buffer [5];
        int buffPos = 0;
        for(i = 0; i < size; i++)  //Size -1 because of the checkSum
        {
            sprintf(buffer, "0x%02X ", packet[i]);
            for(j = 0; j < 4; j++)
            {
                client_message[buffPos] = buffer[j];
                buffPos++;
            }
        }

        // Send the message to server:
        if(send(socket_desc, client_message, strlen(client_message), 0) < 0)
        {
            printf("Unable to send message\n");
            return -1;
        }

        // Receive the server's response:
        if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0)
        {
            printf("Error while receiving server's msg\n");
            return -1;
        }
        printf("Server's response: %s\n",server_message);


        break;
    }
    // Close the socket:
    close(socket_desc);

    return 0;
}
