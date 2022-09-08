#ifndef TRANSMISSION_UTILS_H
#define TRANSMISSION_UTILS_H

#include <stddef.h>

int send_msg(int sockfd, unsigned char *buffer);
int recv_msg(int sockfd, unsigned char *buffer, const size_t buffer_size);

#endif
