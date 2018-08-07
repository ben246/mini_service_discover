#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "./comm/protocal.h"
#include "./comm/udp.h"


static int write_and_get_result(udp_client* client);
static void* udp_broadcast_data(void *arg);



static int check_result(udp_client* client)
{
    service_pkt *pkt = get_pkt();
    int ret = -1;
    
    if(pkt){
        
       while(1){
           if(udp_client_read(client, pkt->data, pkt->len) > 0){
               result_info* res;
               
               client->read_end = 1;
               
               switch(decode_action(pkt)){
                   case INFO_TYPE_ACTION_SUCC:
                        //printf("register succ\n");
                        ret = 0;
                        break;
                   case INFO_TYPE_ACTION_FAIL:
                        res = get_result(pkt);
                        if(res){
                           printf("fail reason:%s\n", 
                                  res->reason);
                            free_result(res);
                        }
                        break;
                   default:
                        printf("invalid action\n");
                        break;
               }
           } else {
               break;
           }
        }
       
       free_pkt(pkt);
    }
    
    return ret;
}

int register_service(char *device_name, char *service_name, char *ip, uint16_t port, char *info)  
{
    int is_broadcast = 0;
    udp_client* client = init_udp_client(is_broadcast);
    int ret = -1;
    
    if(client){        
        service_pkt *pkt = get_register_pkt(device_name, service_name, ip, port, info);
        
        if(pkt){            
            client->data = pkt->data;
            client->len = pkt->len;
        
            ret = write_and_get_result(client);
            
            free_pkt(pkt);
        } else {
            printf("get_register_pkt fail\n");
        }
    }
    
    return ret;
}


int unregister_service(char *device_name, char *service_name)
{
    int is_broadcast = 0;
    udp_client* client = init_udp_client(is_broadcast);
    int ret = -1;
    
    if(client){     
        service_pkt *pkt = get_unregister_pkt(device_name, service_name);
        
        if(pkt){
            client->data = pkt->data;
            client->len = pkt->len;
        
            ret = write_and_get_result(client);
            
            free_pkt(pkt);
        } else {
            printf("get_unregister_pkt fail\n");
        }
    }
  
    return ret;
}



static int write_and_get_result(udp_client* client)
{
    pthread_t pid = 0;
    int ret = 0;
            
    if(client->is_broadcast){
        udp_client_write(client, client->data, client->len);
    } else {
        pthread_create(&pid, NULL, udp_broadcast_data, (void *)client);
    }
                
    ret = check_result(client);
    
    if(client->is_broadcast){
        pthread_join(pid, NULL);
    }
    
    return ret;
}

static void* udp_broadcast_data(void *arg) {
    udp_client* client = (udp_client*)arg;
	int finish = 0;
    
	reset_sub_addr(client);

	while(!finish) {
		if(client->read_end) {
			break;
		}
		finish = prepare_sub_addr(client)!=0;
		udp_client_write(client, client->data, client->len);
	}
    
    return NULL;
}
