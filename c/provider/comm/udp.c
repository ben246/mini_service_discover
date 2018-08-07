#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ifaddrs.h>  
#include <arpa/inet.h>
#include <unistd.h>
#include "udp.h"


udp_server* init_udp_server()
{
    udp_server* server = malloc(sizeof(udp_server));
    bzero(server, sizeof(udp_server));
    
    struct sockaddr_in addrto;  
    bzero(&addrto, sizeof(struct sockaddr_in));  
    addrto.sin_family = AF_INET;  
    addrto.sin_addr.s_addr = htonl(INADDR_ANY); 
    addrto.sin_port = htons(PORT);  
      
    int sock = -1;  
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        printf("init udp server, socket error\n");   
        return NULL;  
    }
  
    if(bind(sock,(struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) == -1){     
        printf("init udp server, bind error...\n");  
        return NULL;  
    }
    
    server->sock = sock;
    server->addr_len = sizeof(struct sockaddr_in);
    return server;
}

void deinit_udp_server(udp_server* server)
{
    if(server){
        close(server->sock);
        free(server);
    }
}


int udp_server_read(udp_server* server, uint8_t *data, int len)
{
    int r_len = 0;
    int ret = 0;
    
    if(!server){
        return 0;
    }
    
	while(1) {
		ret = recvfrom(server->sock, data+r_len, len-r_len, MSG_WAITALL, (struct sockaddr*)&server->addrfrom, &server->addr_len);
        //if(client->addrfrom == client->addrto){
            if (ret <= 0) {
                return ret;
            } else {
                r_len += ret;
                if (r_len >= len) {
                    break;
                }
            }
        //} else {
        //    printf("udp_client_read data from error addr\n");
        //}
	}

    return len;
}

int udp_server_write(udp_server* server, uint8_t *data, int len)
{
	int w_len = 0;
    int ret = 0;
    
    if(!server){
        return 0;
    }
    
	while(1) {
		ret = sendto(server->sock, data+w_len, len-w_len, 0, (struct sockaddr*)&server->addrfrom, server->addr_len);  
		if (ret <= 0) {
			return ret;
		} else {
			w_len += ret;
			if (w_len >= len) {
				break;
			}
		}
	}

    return len;
}

/////////////////////////////////////////////////////////////////////////////////

static int get_local_ipv4_broadcast_addr(char *buf, int size)
{
    struct sockaddr_in *broadcast = NULL;
    struct sockaddr_in *netmask = NULL;
    struct ifaddrs *ifa = NULL, *ifList;

    if (getifaddrs(&ifList) < 0){
        return -1;
    }

    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next){
        if(ifa->ifa_addr->sa_family == AF_INET){

            broadcast = (struct sockaddr_in *)ifa->ifa_broadaddr;
            netmask = (struct sockaddr_in *)ifa->ifa_netmask;
          
            if(!strcmp(inet_ntoa(netmask->sin_addr), "255.255.255.0")){
                strncpy(buf, inet_ntoa(broadcast->sin_addr), size);
                goto exit;
            }
        }
    }
    
    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next){
        if(ifa->ifa_addr->sa_family == AF_INET){

            broadcast = (struct sockaddr_in *)ifa->ifa_broadaddr;
        
            if(!strcmp(ifa->ifa_name, "lo")){
                strncpy(buf, inet_ntoa(broadcast->sin_addr), size);
                goto exit;
            }
        }
    }

exit:
    freeifaddrs(ifList);

    return 0;
}


static int get_local_ipv4_addr(char *buf, int size)
{
    struct ifaddrs *ifa = NULL, *ifList;

    if (getifaddrs(&ifList) < 0){
        return -1;
    }

    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next){
        if(ifa->ifa_addr->sa_family == AF_INET){

            struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
          
            //0.0.0.0
            if(strcmp(ifa->ifa_name, "lo") && 
               strlen(inet_ntoa(addr->sin_addr)) >= 7){
                strncpy(buf, inet_ntoa(addr->sin_addr), size);
                goto exit;
            }
        }
    }
    
    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next){
        if(ifa->ifa_addr->sa_family == AF_INET){

            struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
        
            if(!strcmp(ifa->ifa_name, "lo")){
                strncpy(buf, inet_ntoa(addr->sin_addr), size);
                goto exit;
            }
        }
    }

exit:
    freeifaddrs(ifList);

    return 0;
}


static int set_socket_timeout(int sockid, int timeout_milisecond)
{
    struct timeval timeout={timeout_milisecond/1000, timeout_milisecond*1000};//500ms
    int ret = setsockopt(sockid,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
    if(ret < 0 ){
        printf("setsockopt send fail!\n");
        close(sockid);
        return -1;
    }

    ret = setsockopt(sockid,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
    if(ret < 0 ){
        printf("setsockopt rec fail!\n");
        close(sockid);
        return -1;
    }
 
    return 0;
}


udp_client* init_udp_client(int is_broadcast)
{
    udp_client* client = (udp_client *)malloc(sizeof(udp_client));
    char ip_buf[LOCAL_ADDR_BUF_LEN];
    
    bzero(client, sizeof(udp_client));  
    
    client->is_broadcast = is_broadcast;
    
    int sock = -1;  
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)   
    {     
        printf("socket error\n");   
        return NULL;  
    }
    
    set_socket_timeout(sock, UDP_TIMEOUT);
    
    if(is_broadcast){
        if(!get_local_ipv4_broadcast_addr(ip_buf, sizeof(ip_buf))){
            ;//printf("ip=%s\n", ip_buf);
        }
        
        const int opt = 1;    
        int nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));  
        if(nb == -1)  
        {  
            printf("set socket error...\n");  
            return NULL;  
        }  
        
        client->addrto.sin_addr.s_addr= inet_addr(ip_buf);//htonl(INADDR_BROADCAST);  
    } else {
        char ip[LOCAL_ADDR_BUF_LEN], *p;
        
        bzero(ip,  sizeof(ip));
        get_local_ipv4_addr(ip, sizeof(ip));
        
        p = strrchr(ip, '.'); 
        if(p){
            p++;
            memset(p, 0, LOCAL_ADDR_BUF_LEN-(p-ip));
        }
        memcpy(client->ip, ip, sizeof(ip));
    }
  
    client->sock = sock;
    
    client->addrto.sin_family=AF_INET;  
    client->addrto.sin_port=htons(PORT);  
    client->addr_len=sizeof(client->addrto);  
    
    return client;
}

void deinit_udp_client(udp_client* client)
{
    if(client){
        close(client->sock);
        free(client);
    }
}

int udp_client_read(udp_client* client, uint8_t *data, int len)
{
    int r_len = 0;
    int ret = 0;
    
    if(!client){
        return 0;
    }
    
	while(1) {
		ret = recvfrom(client->sock, data+r_len, len-r_len, MSG_WAITALL, (struct sockaddr*)&client->addrfrom, &client->addr_len);
        //if(client->addrfrom == client->addrto){
            if (ret <= 0) {
                return ret;
            } else {
                r_len += ret;
                if (r_len >= len) {
                    break;
                }
            }
        //} else {
        //    printf("udp_client_read data from error addr\n");
        //}
	}

    return len;
    
}


int udp_client_write(udp_client* client, uint8_t *data, int len)
{
	int w_len = 0;
    int ret = 0;
    
    if(!client){
        return 0;
    }
    
	while(1) {
		ret = sendto(client->sock, data+w_len, len-w_len, 0, (struct sockaddr*)&client->addrto, client->addr_len);  
		if (ret <= 0) {
			return ret;
		} else {
			w_len += ret;
			if (w_len >= len) {
				break;
			}
		}
	}

    return len;
}

void reset_sub_addr(udp_client* client) 
{
	client->sub_ip = 1;
}

int prepare_sub_addr(udp_client* client) 
{
    int ret = 0;
    char ip[LOCAL_ADDR_BUF_LEN];
    
	sprintf(ip, "%s%d", client->ip, client->sub_ip);
	//printf("sub_ip=%s\n", ip);
    
    client->addrto.sin_addr.s_addr= inet_addr(ip);
    
	if(client->sub_ip < 254){
		client->sub_ip++;
	} else {
		ret = -1;
	}
	return ret;
}
