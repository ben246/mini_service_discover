package com.miniservice.discover.provider;

public class ServiceInfo {
     public String dev;
     public String name;
     public String ip;
     public String info;
     public int    port;
     
     public ServiceInfo(){
     }
     
     public ServiceInfo(String dev,
                        String name,
                        String ip,
                        String info,
                       int    port){
         this.dev = dev;
         this.name = name;
         this.ip = ip;
         this.info = info;
         this.port = port;
     }
}
