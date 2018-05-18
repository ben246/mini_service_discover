#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT               9988
#define UDP_COMM_BUF_LEN   256
#define UDP_TIMEOUT        500  //ms


typedef struct udp_client_t{
struct sockaddr_in addrto; 
struct sockaddr_in addrfrom;
int sock;
socklen_t addr_len;
}udp_client;

udp_client* init_udp_client();
void deinit_udp_client(udp_client* client);

int udp_client_read(udp_client* client, uint8_t *data, int len);
int udp_client_write(udp_client* client, uint8_t *data, int len);

#endif
