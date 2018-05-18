#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "protocal.h"
#include "udp.h"


void check_result(udp_client* client)
{
    service_pkt *pkt = get_pkt();
 
    if(pkt){
        
       while(1){
           if(udp_client_read(client, pkt->data, pkt->len) > 0){
               service_info* info;
               result_info* res;
               
               switch(decode_action(pkt)){
                   case INFO_TYPE_ACTION_SUCC:
                        info = get_service_info(pkt);
                        if(info){
                           printf("dev:%s service:%s ip:%s port:%d\n", 
                                  info->device_name,
                                  info->service_name,
                                  info->ip,
                                  info->port);
                            free_service_info(info);
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
    
}

int main()  
{  
    udp_client* client = init_udp_client();
    
    if(client){
        
        service_pkt *pkt = get_inquiry_pkt("play_music");
        if(pkt){
            udp_client_write(client, pkt->data, pkt->len);
            
            check_result(client);
            
            free_pkt(pkt);
        } else {
            printf("get_inquiry_pkt fail\n");
        }
        
        deinit_udp_client(client);
    } else {
        printf("init_udp_client fail\n");
    }
  
    return 0;  
}  
