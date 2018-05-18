#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocal.h"
#include "cJSON.h"
#include "udp.h"



service_pkt *get_pkt()
{
    service_pkt *pkt = (service_pkt *)malloc(sizeof(service_pkt));
    bzero(pkt, sizeof(service_pkt));
    
    pkt->data = (uint8_t *)malloc(UDP_COMM_BUF_LEN);
    bzero(pkt->data, UDP_COMM_BUF_LEN);
    pkt->len = UDP_COMM_BUF_LEN;
    
    return pkt;
}

void free_pkt(service_pkt *pkt)
{
    free(pkt->data);
    free(pkt);
}


service_pkt *get_inquiry_pkt(char *service_name)
{
    service_pkt *pkt = get_pkt();
    
    //content
    service_head* head = (service_head*)pkt->data;
    head->head = INFO_HEAD;
    head->action = INFO_TYPE_INQUIRY;

    cJSON *root = cJSON_CreateObject();  

    cJSON_AddStringToObject(root,"device_name", "");  
    cJSON_AddStringToObject(root,"service_name", service_name);  
    cJSON_AddStringToObject(root,"ip", "");  
    cJSON_AddNumberToObject(root,"port", 0);  
    
    char *out = cJSON_Print(root);  
    //printf("%s\n",out);  
    
    head->info_len = strlen(out);
    memcpy(pkt->data+sizeof(service_head), out, head->info_len);
    
    head->checksum = 0;
    uint8_t *data = out;
    for(uint16_t i=0; i<head->info_len; i++){
        head->checksum += *data++;
    }
    cJSON_Delete(root);  
    free(out);
    
    return pkt;
}

//>0: succ
int checksum_right(service_pkt *pkt)
{
    service_head* head = (service_head*)pkt->data;
    
    uint16_t sum = 0;
    uint8_t *data = pkt->data + sizeof(service_head);
    for(uint16_t i=0; i<head->info_len; i++){
        sum += *data++;
    }
    
    return sum == head->checksum;
}

ACTION_TYPE decode_action(service_pkt *pkt)
{
    if(checksum_right(pkt)){
        service_head* head = (service_head*)pkt->data;
        return head->action;
    } else {
        return INFO_TYPE_ACTION_INVALID;
    }
}

service_info* get_service_info(service_pkt *pkt)
{
    service_head* head = (service_head*)pkt->data;
    const char *json_data = (const char *)(pkt->data + sizeof(service_head));
    service_info* info;
    
    if(head->action == INFO_TYPE_ACTION_SUCC){
        
        cJSON *json, *json_value;  
          
        json = cJSON_Parse(json_data);  
        if (!json)  
        {  
            printf("Error before: [%s]\n",cJSON_GetErrorPtr());  
            return NULL;
        }  
        else  
        {  
            info = (service_info *)malloc(sizeof(service_info));
            bzero(info, sizeof(service_info));
    
            json_value = cJSON_GetObjectItem( json , "device_name");  
            if(json_value && json_value->type == cJSON_String )  {  
                info->device_name = strdup(json_value->valuestring);
            } 
            json_value = cJSON_GetObjectItem( json , "service_name");  
            if(json_value && json_value->type == cJSON_String )  {  
                info->service_name = strdup(json_value->valuestring);
            } 
            json_value = cJSON_GetObjectItem( json , "ip");  
            if(json_value && json_value->type == cJSON_String )  {  
                info->ip = strdup(json_value->valuestring);
            } 
            json_value = cJSON_GetObjectItem( json , "port");  
            if(json_value && json_value->type == cJSON_Number )  {  
                info->port = json_value->valueint;
            } 
            
            cJSON_Delete(json);  
            return info;
        }  
    
    } 
   
    return NULL;
}

void free_service_info(service_info *info)
{
    if(info->device_name)
       free(info->device_name);
    if(info->service_name)
       free(info->service_name);
    if(info->ip)
       free(info->ip);
    free(info);
}

result_info* get_result(service_pkt *pkt)
{
    service_head* head = (service_head*)pkt->data;
    const char *json_data = (const char *)(pkt->data + sizeof(service_head));
    result_info* res;
    
    if(head->action == INFO_TYPE_ACTION_FAIL){
        
        cJSON *json, *json_value;  
          
        json = cJSON_Parse(json_data);  
        if (!json)  
        {  
            printf("Error before: [%s]\n",cJSON_GetErrorPtr());  
            return NULL;
        }  
        else  
        {  
            res = (result_info *)malloc(sizeof(result_info));
            bzero(res, sizeof(result_info));
    
            json_value = cJSON_GetObjectItem( json , "reason");  
            if(json_value && json_value->type == cJSON_String )  {  
                res->reason = strdup(json_value->valuestring);
            } 
            
            cJSON_Delete(json);  
            return res;
        }  
    
    }
    
    return NULL;
}

void free_result(result_info *res)
{
    if(res->reason)
       free(res->reason);
    free(res);
}
