#include <stdio.h>
#include <unistd.h>
#include "server.h"


int main()  
{
    int ret;

    printf("register box1, play_music\n");
    ret = register_service("box1", "play_music", "192.168.1.116", 6666, "play music with pcm data fskjdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddfklsaaaaaaaaaaaaaaa");
    if(!ret){
        printf("succ\n");
    } else {
        printf("fail\n");
    }

    printf("register box2, play_music\n");
    ret = register_service("box2", "play_music", "192.168.1.116", 6666, "play music with acc data");
    if(!ret){
        printf("succ\n");
    } else {
        printf("fail\n");
    }
    
    sleep(6);
    
    printf("unregister box2, play_music\n");
    ret = unregister_service("box2", "play_music");
    if(!ret){
        printf("succ\n");
    } else {
        printf("fail\n");
    }
        
  
    return 0;  
}
