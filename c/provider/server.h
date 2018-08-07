
#ifndef _CLIENT_H
#define _CLIENT_H

#include "./comm/list.h"



int register_service(char *device_name, char *service_name, char *ip, uint16_t port, char *info)  ;
int unregister_service(char *device_name, char *service_name);




#endif
