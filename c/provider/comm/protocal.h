#ifndef PROTOCAL_H
#define PROTOCAL_H

#include <stdint.h>

#define INFO_HEAD  0xFFEE
#define INFO_MAX_LEN  (100)

typedef enum
{
INFO_TYPE_REGISTER_SERVICE  = 0,
INFO_TYPE_UNREGISTER_SERVICE,
INFO_TYPE_INQUIRY,
INFO_TYPE_ACTION_SUCC,
INFO_TYPE_ACTION_FAIL,

INFO_TYPE_ACTION_INVALID = -1,
}ACTION_TYPE;


typedef  struct service_head_t
{
uint16_t head;
uint16_t action;
uint16_t info_len;
uint16_t checksum;
}service_head;

typedef  struct service_pkt_t
{
uint8_t *data;
int len;
}service_pkt;

typedef struct service_info_t
{
char *device_name;
char *service_name;
char *ip;
char *info;
uint16_t port;
}service_info;


typedef struct result_info_t
{
char *reason;
}result_info;


#define   SERVICE_NO_FOUND   "service no found"
#define   REGISTER_SUCC      "register succ"


service_pkt *get_pkt();
void free_pkt(service_pkt *pkt);

service_pkt *get_register_pkt(char *device_name, char *service_name, char *ip,uint16_t port, char *info);
service_pkt *get_unregister_pkt(char *device_name, char *service_name);
service_pkt *get_inquiry_pkt(char *service_name);
service_pkt *get_inquiry_result_pkt(service_info *info);
service_pkt *get_result_pkt(int is_succ, char *reason);


ACTION_TYPE decode_action(service_pkt *pkt);

service_info* get_service_info(service_pkt *pkt);
void free_service_info(service_info *info);

result_info* get_result(service_pkt *pkt);
void free_result(result_info *res);

#endif

