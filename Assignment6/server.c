#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 8081
#define BUFFER_SIZE 1024

struct Request
{
    int student_id;
    char file_data[BUFFER_SIZE];
};

void handle_client(int new_socket)
{
    struct Request request;
    char response[BUFFER_SIZE];

    // Handle multiple queries from same client
    while (1)
    {
        memset(&request, 0, sizeof(request));

        // Receive request
        int n = read(new_socket,
                     &request,
                     sizeof(request));

        if (n <= 0)
        {
            printf("Client disconnected.\n");
            break;
        }

        printf("Child %d received Student ID: %d\n",
               getpid(),
               request.student_id);

        // Search Student ID
        char id[20];

        sprintf(id, "%d", request.student_id);

        char *line = strtok(request.file_data, "\n");

        int found = 0;

        while (line != NULL)
        {
            char line_id[20];

            sscanf(line, "%s", line_id);

            if (strcmp(line_id, id) == 0)
            {
                sprintf(response,
                        "Record Found: %s",
                        line);

                found = 1;
                break;
            }

            line = strtok(NULL, "\n");
        }

        // If ID not found
        if (!found)
        {
            strcpy(response, "Record Not Found");
        }

        // Send response
        write(new_socket,
              response,
              strlen(response) + 1);
    }

    close(new_socket);

    exit(0);
}

int main()
{
    int server_fd;
    int new_socket;

    struct sockaddr_in address;

    int addrlen = sizeof(address);

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    // 2. Define address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 3. Bind
    if (bind(server_fd,
             (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("Bind failed");
        return 1;
    }

    // 4. Listen
    listen(server_fd, 3);

    printf("Server waiting on port %d...\n", PORT);

    // 5. Continuously accept clients
    while (1)
    {
        new_socket = accept(server_fd,
                            (struct sockaddr *)&address,
                            (socklen_t *)&addrlen);

        if (new_socket < 0)
        {
            perror("Accept failed");
            continue;
        }

        printf("Client connected.\n");

        // 6. Create child
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("Fork failed");
            close(new_socket);
        }

        else if (pid == 0)
        {
            // CHILD

            close(server_fd);

            handle_client(new_socket);
        }

        else
        {
            // PARENT

            close(new_socket);

            // Avoid zombie processes
            waitpid(-1, NULL, WNOHANG);
        }
    }

    close(server_fd);

    return 0;
}