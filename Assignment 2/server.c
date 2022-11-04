// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <pwd.h>

#define PORT 80
char *hello = "Hello from server";
void send_message(const char* new_socket_str);
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    pid_t currentPid;
    int value;
    struct passwd* id;

    if(argc == 1) {
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // Attaching socket to port 80
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                                                    &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );

        // Forcefully attaching socket to the port 80
        if (bind(server_fd, (struct sockaddr *)&address,
                                    sizeof(address))<0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                        (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        currentPid = fork(); //create child process and return pid of child to parent
        if(currentPid == 0) {
            //Assignment 2: Re-exec the server’s child process
       		printf("Child process id : %d \n", getuid());
       		printf("Exec the server's child process\n");
       		char socketIdStr[20];
       		sprintf(socketIdStr, "%d", new_socket);
            // Pass the socket id to the new exec'ed child from the forked child
		    char *args[] = {"./server", socketIdStr, NULL}; 
            if(execvp(args[0], args) < 0) {
                perror("Exec failed");
                exit(EXIT_FAILURE);
		    }
        } else if(currentPid > 0) {
            wait(NULL); // Wait until child process is created
        } else {
            printf("Fork failed\n");
            _exit(2);
        }
    } else{
        printf("Child process id before privilege drop: %d \n", getuid());
        id = getpwnam("nobody"); //get id of nobody user      
        
        value = setuid(id -> pw_uid); // dropping privileges for child process to nobody user
        printf("Child process id after privilege drop: %d\n", getuid());

        if(value == 0) {
            // Sending message with the socket id passed as argument
            send_message(argv[1]);
        
        } else {
            printf("Dropping privileges failed for child process\n");
            return 0;
        }
    }
    return 0;
}
void send_message(const char* new_socket_str) {

	int new_socket, valread;
	char buffer[1024] = {0};
	new_socket = atoi(new_socket_str);
	valread = read( new_socket , buffer, 1024);
	if(valread < 0) {
		perror("read failed\n");
		exit(EXIT_FAILURE);
	}
    printf("%s\n",buffer );
    send(new_socket , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
}
