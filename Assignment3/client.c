#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
# define BUFFER_SIZE 1024


int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Define server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // inet_pton(AF_INET, "10.158.201.130", &serv_addr.sin_addr);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // 3. Connect to server
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    char msg[BUFFER_SIZE];

    while (1)
    {
        printf("Client: ");
        // scanf("%99s", msg);
        fgets ( msg , BUFFER_SIZE , stdin );
        

        send(sock, msg, strlen(msg), 0);

         if ((strcmp(msg, "bye\n")) == 0){
            printf("Thank u for using our app..");
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        read(sock, buffer, sizeof(buffer));
        

        if ((strcmp(buffer, "bye\n")) == 0){
            printf("Thank u for using our app..");
            break;
        }
        printf("Server: %s\n", buffer);

    }

    close(sock);

    return 0;
}