#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "list.h"


service_info_node* get_node()
{
    service_info_node* node = malloc(sizeof(service_info_node));
    bzero(node, sizeof(service_info_node));
    return node;
}


service_info_list* init_list()
{
    service_info_list* list = malloc(sizeof(service_info_list));
    bzero(list, sizeof(service_info_list));
    return list;
}

void deinit_list(service_info_list* list)
{
    service_info_node* next;
    service_info_node*  node = list->head;
    
    while(node){
        next = node->next;
        
        if(node->info){
           free(node->info);
        }
        free(node);
        
        node = next;
    }
    
    free(list);
}


void list_add(service_info_list* list, service_info* info)
{
    service_info_node* node = get_node();
    node->info = info;
    node->next = NULL;
 
    //remove same service
    list_remove(list, info->device_name, info->service_name);
    
    if(!list->head){
       list->head = node;
    }
   
    if(!list->tail){
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
}

int list_remove(service_info_list* list, char* device_name, char* service_name)
{
    service_info_node *next = NULL;
    service_info_node *prev = NULL;
    service_info_node* node = list->head;
    int remove = 0;
    
    while(node){
        next = node->next;
        
        if(node->info){
           if(!strcmp(node->info->device_name, device_name) &&
              !strcmp(node->info->service_name, service_name)){
               remove = 1;
               //printf("remove dev=%s name=%s", device_name, service_name);
               if(prev){
                   if(next){
                      prev->next = next;
                   } else {               //node is tail
                      prev->next = NULL;   
                      list->tail = prev;
                   }
               } else {                   //node is head
                   list->head = next;     
               }
               break;
           }
        }
        
        prev = node;
        node = next;
    }
    
    return remove?0:-1;
}

//input: index(0 - ...)
service_info* list_get(service_info_list* list, char* service_name, const int index)
{
    service_info_node*  next;
    service_info_node*  node = list->head;
    int i = 0;
    
    while(node){
        next = node->next;
        
        if(node->info && i== index){
           if(!strcmp(node->info->service_name, service_name)){
               //printf("index=%d i=%d\n", index, i);
               return node->info;
           }
        }
        
        node = next;
        i++;
    }
    
    return NULL;
}

