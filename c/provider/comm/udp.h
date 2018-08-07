#ifndef UDP_LINK_H
#define UDP_LINK_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT               9988
#define UDP_COMM_BUF_LEN   1024
#define UDP_TIMEOUT        500  //ms
#define LOCAL_ADDR_BUF_LEN  (30)

/////////////////////////////////////////////////////////////////////////////////

typedef struct udp_server_t{
struct sockaddr_in addrfrom;  
int sock;
socklen_t addr_len;
}udp_server;

udp_server* init_udp_server();
void deinit_udp_server(udp_server* server);

int udp_server_read(udp_server* server, uint8_t *data, int len);
int udp_server_write(udp_server* server, uint8_t *data, int len);

/////////////////////////////////////////////////////////////////////////////////
typedef struct udp_client_t{
struct sockaddr_in addrto; 
struct sockaddr_in addrfrom;
char ip[LOCAL_ADDR_BUF_LEN];
int is_broadcast;
int sub_ip;
int read_end;
uint8_t *data;
int len;
int sock;
socklen_t addr_len;
}udp_client;

udp_client* init_udp_client(int is_broadcast);
void deinit_udp_client(udp_client* client);

int udp_client_read(udp_client* client, uint8_t *data, int len);
int udp_client_write(udp_client* client, uint8_t *data, int len);
void reset_sub_addr(udp_client* client);
int prepare_sub_addr(udp_client* client); 

#endif
