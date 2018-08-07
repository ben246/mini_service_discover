#include <stdio.h>
#include "client.h"


int main(int argc, char *argv[])
{
    service_info_list* list = inquiry_service("play_music");
    service_info_node* node;
    service_info* info;
    
    if(list != NULL){
        
        node = list->head;
        while(node){
            info = node->info;
            printf("play_music: %s, %s, %d, %s\n", info->device_name, info->ip, info->port, info->info);

            node = node->next;
        }
    
        deinit_list(list);
    } else {
        printf("inquiry fail\n");
    }
    
    return 0;
}


