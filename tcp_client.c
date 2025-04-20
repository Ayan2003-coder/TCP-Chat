#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


#define MAX_BUFFER_SIZE 1024
void handler(int num)
{
	exit(0);
}
int main() 
{
    signal(SIGINT,handler);
    int client_socket;
    struct sockaddr_in server_address;
    char message[MAX_BUFFER_SIZE];
    int bytes_received;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address struct
    int port;char ip[20];
    printf("Enter the port no of server: ");
    scanf("%d",&port);
    printf("Enter the IP address of the server: ");
    scanf("%s",ip);
    sleep(1);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip); // Server IP address

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
    {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }
    pid_t parent=getpid();
    pid_t pid=fork();
    if(pid!=0)
    {
	    while(1)
	    {
	       // Send message to server
                fgets(message, sizeof(message), stdin);
                send(client_socket, message, strlen(message), 0);
	   }

   }
    while (1) 
    {
        // Receive response from server
        bytes_received = recv(client_socket, message, sizeof(message), 0);
        if (bytes_received <= 0) 
	{
           break;
        }

        message[bytes_received-1] = '\0';
        printf("%s", message);
	printf("\n");
    }

   return 0;
}