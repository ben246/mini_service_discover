#ifndef _LIST_H
#define _LIST_H

#include "protocal.h"


typedef struct service_info_node_t{
service_info *info;
struct service_info_node_t *next;
}service_info_node;

typedef struct service_info_list_t{
service_info_node *head;
service_info_node *tail;
}service_info_list;



service_info_list* init_list();
void deinit_list(service_info_list* list);

void list_add(service_info_list* list, service_info* info);
int  list_remove(service_info_list* list, char* device_name, char* service_name);

service_info* list_get(service_info_list* list, char* service_name, const int index);

#endif
