#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../headers/transmission_utils.h"
#include "../headers/webserver.h"

#define PORT 80
#define MSG_SIZE 512
#define RESOURCE_DIR "../resources"

static int create_socket(void)
{
    int sockfd;
    const int flag = 1;

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation");
        exit(-1);
    }
        
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1) {
        perror("setting socket option SO_REUSEADDR");
        exit(-1);
    }

    return sockfd;
}

static struct sockaddr_in *set_host_address(void)
{
    struct sockaddr_in *host_addr = malloc(sizeof(struct sockaddr_in));
    if (!host_addr) {
        perror("Host address setup failed");
        exit(-1);
    }

    host_addr->sin_family = AF_INET;
    host_addr->sin_port = htons(PORT);
    host_addr->sin_addr.s_addr = INADDR_ANY; 
    memset(&(host_addr->sin_zero), '\0', 8);

    return host_addr;
}

static int bind_and_listen(int sockfd, struct sockaddr_in *host_addr)
{
    if (bind(sockfd, (struct sockaddr *)host_addr, sizeof(struct sockaddr)) == -1) {
        perror("binding to socket");
        exit(-1);
    }

    if (listen(sockfd, 20) == -1) {
        perror("listening on socket");
        exit(-1);
    }

    return 0;
}

static int get_file_size(int fd)
{
    struct stat stat_struct;

    if (fstat(fd, &stat_struct) == -1) {
        return -1;
    }
    return (int) stat_struct.st_size;
}

static void handle_connection(int sockfd, struct sockaddr_in *client_addr)
{
    int resource_fd;
    unsigned char request[MSG_SIZE], resource[MSG_SIZE];
    int length = recv_msg(sockfd, request);

    printf (
        "Received request from %s:%d \'%s'\"\n", 
        inet_ntoa(client_addr->sin_addr),
        ntohs(client_addr->sin_port), 
        request
    );

    unsigned char *ref = strstr(request, " HTTP/");
    if (!ref) {
        printf("Did not receive HTTP request");
    } else {
        *ref = 0;
        ref = NULL;

        if (strncmp(request, "GET ", 4) == 0) {
            ref = request + 4;
        }

        if (strncmp(request, "HEAD ", 5) == 0) {
            ref = request + 5;
        }

        if (ref == NULL) {
            printf("Unknown request !\n");
        } else {
            if (ref[strlen(ref) - 1] == '/') {
                strcat(ref, "index.html");
            }

            strcpy(resource, RESOURCE_DIR);
            strcat(resource, ref);

            resource_fd = open(resource, O_RDONLY, 0);
            printf("\tAttempting to open \'%s\'\t", resource);

            if (resource_fd == -1) {
                printf(" ERROR 404 - RESOURCE NOT FOUND\n");
                send_msg(sockfd, "HTTP/1.0 404 NOT FOUND\r\n");
                send_msg(sockfd, "Server: \r\n\r\n");
                send_msg(sockfd, "<html><head><title>404 Not Found</title></head>");
                send_msg(sockfd, "<body><h1>URL not found</h1></body></html>\r\n");
            } else {
                printf(" 200 OK\n");
                send_msg(sockfd, "HTTP/1.0 200 OK\r\n");
                send_msg(sockfd, "Server: \r\n\r\n");

                if (ref == request + 4) {
                    length = get_file_size(resource_fd);

                    if (length == -1) {
                        perror("getting resource file size");
                        exit(-1);
                    }

                    ref = (unsigned char *) malloc(length);
                    if (!ref) {
                        perror("allocating memory for resource");
                        exit(-1);
                    }

                    read(resource_fd, ref, length);
                    send(sockfd, ref, length, 0);
                    free(ref);
                }

                close(resource_fd);
            }
        }
    }

    shutdown(sockfd, SHUT_RDWR);
}

static void loopback(int sockfd)
{
    socklen_t sin_size;
    int new_sockfd;
    struct sockaddr_in client_addr;

    for (;;) { 
        sin_size = sizeof(struct sockaddr_in);
        printf("\n\nWaiting for incoming connections ...\n");
        new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if(new_sockfd == -1) {
            perror("accepting connection");
            exit(-1);
        }
        handle_connection(new_sockfd, &client_addr);
    }
}

void activate_webserver(void)
{
    int sockfd = create_socket();
    struct sockaddr_in *host_addr = set_host_address();
    bind_and_listen(sockfd, host_addr);
    loopback(sockfd);
}