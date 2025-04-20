Skip to main content
Google Classroom
Classroom
Computer Network Laboratory (CS3272)
GX & GY
Home
Calendar
Enrolled
To-do
Q
QCUG25
A
Artificial Intelligence
8th Semester
N
NLD: Chaos and Fractals-2025
Physics
A
Artificial Intelligence Laboratory
8th Semester 2025
E
Embedded Systems
C
Computer Network Laboratory (CS3272)
GX & GY
D
Data Communication and Computer Network (CS3202)
GX and GY - 2024
S
SaS Lab 22
W
WEB TECHNOLOGY
P
PROGRAMMING PARADIGM
C
CS2272-2023
CST
C
CS2202-2023
CST
D
Data Science-2023
4th Semester
H
HOME ASSIGNMENT B TECH 3RD SEMESTER
COMPUTER SCIENCE AND TECHNOLOGY
D
Discrete Structures (CS 2101)
CST BTech 2nd year, 3rd Semester (Odd Semester 2022-23)
2
2022 Group V
U
UG Physics PH1201 Group-V Theory 2022 May paper recheck
E
End Sem Exam Gr-V (UG 1st Sem)
Group-V
U
UG 1st semester 2021-2022 Gr - V
Gr - V
U
UG 1st Sem Group V MidTerm Exam Feb 2022
Group V
B
B.Tech. (1st Semester, Group-V) Mid -Term Examination, Feb. 2022
Group-V
B
B.tech 2022
Gr. V
P
PT/NCC CLASS
GROUP-V
B
BTECH
5
G
Group V ( 1st Sem B. Tech.) Dec 2021-Prof. T. K. Kar
2
2021_1st Sem_UG_Group-V_Drawing_Sessional
Group-V
Archived classes
Settings
Assignment details
Assignment 3: TCP Socket Application
ASHISH KUMAR LAYEK
•
Jan 19, 2024 (Edited Apr 15, 2024)
Due Feb 5, 2024, 11:59 PM

Assignment3.pdf
PDF
Class comments
Your work
Turned in

qn3server.c
C

qn3client.c
C

qn2server.c
C

qn2client.c
C

qn1server.c
C

qn1client.c
C

qn3_output_server-as-mediator.png
Image

qn3_output_2clients-chatting.png
Image

output_qn2.png
Image

output_qn1.png
Image
Work cannot be turned in after the due date
Private comments
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#define PORT 8080
#define MAX_BUFFER_SIZE 1024
int client_socket;int server_socket;
struct client_info
{
	int no_users;
	int socketFD[10];
	char username[10][100];
};
struct client_info* shared_mem;int shmid;
void handler(int num)
{
	printf("Server closed\n");
	close(server_socket);
	shmdt(shmat(shmid,NULL,0));
	shmctl(shmid,IPC_RMID,NULL);
	exit(0);
}

void print(int socket)
{
	char buffer[1024];
	strcpy(buffer,"User list\n\0");
	send(socket,buffer,strlen(buffer),0);
	char socketno[10];
	for(int i=0;i<shared_mem->no_users;++i)
	{
		sprintf(socketno,"%d",shared_mem->socketFD[i]);
		strcpy(buffer,socketno);
		strcat(buffer," : ");
		strcat(buffer,shared_mem->username[i]);
		strcat(buffer,"\n");
		send(socket,buffer,strlen(buffer),0);
	}

}


int useridx(int socket)
{
	int ans;
	for(int i=0;i<shared_mem->no_users;++i)
	{
		if(socket==shared_mem->socketFD[i])
			ans=i;		
	}
	return ans;
}

void* handle_client(void* arg) {
    signal(SIGINT,handler);
    int* client_socketptr=(int*) arg;
    int client_socket=*client_socketptr;
    char buffer[MAX_BUFFER_SIZE];int bytes_received;char msg[MAX_BUFFER_SIZE+100];

    //initialize the client
    recv(client_socket,buffer,sizeof(buffer),0);
    strcpy(buffer,"Enter your name: \0");
    send(client_socket,buffer,strlen(buffer),0);
    bytes_received=recv(client_socket,buffer,sizeof(buffer),0);
    buffer[bytes_received-1]='\0';
    shared_mem->socketFD[shared_mem->no_users]=client_socket;
    strcpy(shared_mem->username[shared_mem->no_users],buffer);
    shared_mem->no_users+=1;



    //connected message and client formality
    strcpy(buffer,"You have connected to the server.\nThe format of sending message is <receiver_socketFD>/<message>\n\0");
    send(client_socket,buffer,strlen(buffer),0);
    strcpy(buffer,"To know available users send 'user' to server\n\0");
    send(client_socket,buffer,strlen(buffer),0);



    while (1) 
	{
            // Receive message from client
            bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0)
	    {
                 break;
            }
	    
            buffer[bytes_received-1] = '\0';
            printf("Message received: %s\n",buffer);
	    if(strcmp(buffer,"user")==0)
		    print(client_socket);
	    else
	    {
	    	char receiver_socket_FD[20];
	    	for(int i=0;i<bytes_received-1;++i)
	    	{
		    if(buffer[i]=='/')
	            {
			    int j=0;
			    i++;
			    while(i<bytes_received-1)
			    {
				buffer[j++]=buffer[i++];
		            }
			    break;
	            }
		    receiver_socket_FD[i]=buffer[i];
	   	 }

	    	int receiverFD=atoi(receiver_socket_FD);
	    	printf("receiver:%d\n",receiverFD);
	    	buffer[strlen(buffer)-1]='\0';
	    	printf("sent:%s\n",buffer);
		strcpy(msg,shared_mem->username[useridx(client_socket)]);
		strcat(msg,": ");
		strcat(msg,buffer);
	    	send(receiverFD,msg,strlen(msg),0);


           }
		   
    }
}

int main() {
   signal(SIGINT,handler);

   //create the shared memory containing the user name and socket_FD;
   shmid=shmget(IPC_PRIVATE,sizeof(struct client_info*),IPC_CREAT|0666);
   shared_mem=(struct client_info*)shmat(shmid,NULL,0);
   shared_mem->no_users=0;


   struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address struct
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accept connections and handle clients
    while (1) {
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len)) == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_address.sin_addr), client_socket);
	sleep(0.5);
	pthread_t tid;
	pthread_create(&tid,NULL,handle_client,(void *)&client_socket);


   
    }


    return 0;
}

qn3server.c
Displaying qn3server.c.