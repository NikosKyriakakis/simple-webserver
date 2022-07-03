#ifndef TRANSMISSION_UTILS_H
#define TRANSMISSION_UTILS_H

int send_msg(int sockfd, unsigned char *buffer);
int recv_msg(int sockfd, unsigned char *buffer);

#endif