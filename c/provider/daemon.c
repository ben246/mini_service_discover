#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "./comm/protocal.h"
#include "./comm/list.h"
#include "./comm/udp.h"


static void server_loop(udp_server* server)
{
    service_pkt *rec_pkt = get_pkt();
    service_info_list *list = init_list();
    
    if(rec_pkt && list){
        
       while(1){
           
           if(udp_server_read(server, rec_pkt->data, rec_pkt->len) > 0){
               
               service_pkt*  pkt;
               service_info *info, *ret_info;
               int i, valid, is_get_service = 0;
               ACTION_TYPE type;
               char *action_str;
               
               valid = 1;
               type = decode_action(rec_pkt);
               switch(type){
                   case INFO_TYPE_REGISTER_SERVICE:
                        action_str = "register";
                        break;
                   case INFO_TYPE_UNREGISTER_SERVICE:
                        action_str = "unregister";
                        break;
                   case INFO_TYPE_INQUIRY:
                        action_str = "inquiry";
                        break;
                   default: 
                        valid = 0;
                        break;
               }
               if(!valid){
                   printf("invalid pkt type\n");
                   continue;
               }
               
               info = get_service_info(rec_pkt);
               if(info){
                   if(type != INFO_TYPE_INQUIRY){
                       printf("%s dev:%s, service:%s, ip:%s, port:%d info:%s\n", 
                              action_str, info->device_name, info->service_name,
                              info->ip, info->port, info->info);
                   } else {
                       printf("%s dev:%s, service:%s\n", 
                              action_str, info->device_name, info->service_name);
                   }
                   switch(type){
                   case INFO_TYPE_REGISTER_SERVICE:
                        list_add(list, info);
                        pkt = get_result_pkt(1, "");
                        if(pkt){
                            udp_server_write(server, pkt->data, pkt->len);
                            free_pkt(pkt);
                        }
                        break;
                   case INFO_TYPE_UNREGISTER_SERVICE:
                        i = list_remove(list, info->device_name, info->service_name);
                        free_service_info(info);  
                        if(i>=0){   
                            pkt = get_result_pkt(1, "");
                        } else {
                            pkt = get_result_pkt(0, SERVICE_NO_FOUND);
                        }
                        if(pkt){
                            udp_server_write(server, pkt->data, pkt->len);
                            free_pkt(pkt);
                        }
                        break;
                   case INFO_TYPE_INQUIRY:
                        i = 0;
                        while(1){
                           ret_info = list_get(list, info->service_name, i++);
                           if(ret_info){
                               pkt = get_inquiry_result_pkt(ret_info);
                               if(pkt){
                                    udp_server_write(server, pkt->data, pkt->len);
                                    free_pkt(pkt);
                                    is_get_service = 1;
                               }
                           } else {
                               break;
                           }
                        }
                        if(!is_get_service){
                           pkt = get_result_pkt(0, SERVICE_NO_FOUND);
                           if(pkt){
                                udp_server_write(server, pkt->data, pkt->len);
                                free_pkt(pkt);
                           }
                        }
                        free_service_info(info);
                        break;
                   default:
                        printf("invalid action\n");
                        free_service_info(info);
                        break;
                   }
               }
           } else {
               break;
           }
        }
       
       free_pkt(rec_pkt);
    } else {
       printf("init server loop fail\n");
    }
}


int daemon_loop()  
{  
    udp_server* server = init_udp_server();
     
    if(server){
        server_loop(server);
        deinit_udp_server(server);
    } else {
        printf("init_udp_server fail\n");
    }
  
    return 0;  
}

