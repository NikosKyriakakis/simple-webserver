#include "../headers/transmission_utils.h"

#include <sys/socket.h>
#include <stdio.h>
#include <string.h>

int send_msg(int sockfd, unsigned char *buffer)
{
    int bytes_sent;
    int bytes_to_send = strlen(buffer);

    while (bytes_to_send > 0) {
        bytes_sent = send(sockfd, buffer, bytes_to_send, 0);
        if (bytes_sent == -1) {
            return 0;
        }
        
        bytes_to_send -= bytes_sent;
        buffer += bytes_sent;
    }

    return 1;
}

int recv_msg(int sockfd, unsigned char *buffer)
{
    unsigned char *buffer_ref = buffer;

    int eol_index = 0;

    const char *EOL = "\r\n";
    const unsigned char EOL_SIZE = 2;

    while (recv(sockfd, buffer_ref, 1, 0) == 1) {
        if (*buffer_ref == EOL[eol_index]) {
            if (++eol_index == EOL_SIZE) {
                *(buffer_ref + 1 - EOL_SIZE) = '\0';
            }
            return strlen(buffer);
        } else {
            eol_index = 0;
        }
        buffer_ref++;
    }

    return 0;
}