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

    printf("execve=0x%p\n", execve);

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
    //Assignment 1: Privilege separation starts here
    currentPid = fork(); //create child process and return pid of child to parent
    if(currentPid == 0) {
       printf("Child process id before privilege drop: %d \n", getuid());

       id = getpwnam("nobody"); //get id of nobody user      
    
       value = setuid(id -> pw_uid); // dropping privileges for child process to nobody user
       printf("Child process id after privilege drop: %d\n", getuid());

       if(value == 0) {
		valread = read( new_socket , buffer, 1024);
		printf("%s\n",buffer );
		send(new_socket , hello , strlen(hello) , 0 );
		printf("Hello message sent\n");
       
       } else {
         printf("Dropping privileges failed for child process\n");
         return 0;
       }
    } else if(currentPid > 0) {
       wait(NULL); // Wait until child process is created
    } else {
       printf("Fork failed\n");
       _exit(2);
    }
    return 0;
}
