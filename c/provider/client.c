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
#include "./comm/list.h"

static service_info_list* write_and_get_result(udp_client* client);
static void* udp_broadcast_data(void *arg);


static service_info_list* check_result(udp_client* client)
{
    service_pkt *pkt = get_pkt();
    service_info_list* list = NULL;
 
    if(pkt){
       list = init_list();
       
       while(1){
           if(udp_client_read(client, pkt->data, pkt->len) > 0){
               service_info* info;
               result_info* res;
               
               client->read_end = 1;
               
               switch(decode_action(pkt)){
                   case INFO_TYPE_ACTION_SUCC:
                        info = get_service_info(pkt);
                        if(info){
                           /*printf("dev:%s, service:%s, ip:%s, port:%d info:%s\n", 
                                  info->device_name,
                                  info->service_name,
                                  info->ip,
                                  info->port,
                                  info->info);*/
                           list_add(list, info);
                        }
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
    
    return list;
}

service_info_list* inquiry_service(char * name)
{  
    int is_broadcast = 0;
    udp_client* client = init_udp_client(is_broadcast);
    service_info_list* list = NULL;
    
    if(client){
        
        service_pkt *pkt = get_inquiry_pkt(name);
        if(pkt){
            client->data = pkt->data;
            client->len = pkt->len;
        
            list = write_and_get_result(client);
            
            free_pkt(pkt);
        } else {
            printf("get_inquiry_pkt fail\n");
        }

        deinit_udp_client(client);
    } else {
        printf("init_udp_client fail\n");
    }
    
    return list;
}


static service_info_list* write_and_get_result(udp_client* client)
{
    pthread_t pid = 0;
    service_info_list* list = NULL;
            
    if(client->is_broadcast){
        udp_client_write(client, client->data, client->len);
    } else {
        pthread_create(&pid, NULL, udp_broadcast_data, (void *)client);
    }
                
    list = check_result(client);
    
    if(client->is_broadcast){
        pthread_join(pid, NULL);
    }
    
    return list;
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

