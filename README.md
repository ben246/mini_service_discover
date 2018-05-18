# mini_service_discover

1.purpose   
> discover the service in the local area network of ipv4 easily
   
2.implementation   
> the process uses the udp broadcast to communicate and the protocal consists of a struct head and a json,   
>the udp data packet is unencrypted.   
    
3.process and language detail   
>   daemon         - the holder of service infomation,           golang   
>   server         - example which providers some service,       golang   
>   client         - example which wants to search some service, golang   
>   client_c       - like client, C code   
>   client_android - like client, Android java   
     
4.attention   
>   1)turn off your firewall if you don't know edit the related rule   
>   2)the process will use the only one network to communicate, if it checks the ipv4 and gets failure,   
>     then it connects to loop addr.      
>   3)client_android must only use wifi for udp connenting and its a eclipse project with adt.   
>   4)linux command to debug   
>      netstat -apnu4 (udp4)   
>   5)testing platform    
>     daemon, server, client, client_c - ubuntu 16.04   
>     client_android  - android 5.1.1   
>   6)this project is developed for interest, if you use it for any other purpose,   
>     you must be responsible for all the consequences.    

    
