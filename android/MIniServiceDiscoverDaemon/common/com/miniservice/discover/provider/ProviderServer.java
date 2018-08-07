package com.miniservice.discover.provider;


import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

import android.content.Context;
import android.util.Log;

public class ProviderServer {
    private final static String TAG = "DiscoverServer";
    UDPServer udp;
    Protocal protocal;
    ServiceInfoList provider;
    Runnable r;
    
    public ProviderServer(Context context){
        udp = new UDPServer(context);
        protocal = new Protocal();
        provider = new ServiceInfoList();
    }
    
    public void run(){
         r = new Runnable() {
            @Override
            public void run() {
                loop();
            }
        };
        new Thread(r).start();
    }

    private void loop(){
        byte[] buffer = new byte[Protocal.PROTOCAL_LEN];
        ServiceInfo info, out_info;
        String reason;
        
        Log.i(TAG, "loop");
        
        while(true){
            
            if( udp.receive(buffer) > 0){
                switch(protocal.decodeAction(buffer)){
                case Protocal.INFO_TYPE_REGISTER_SERVICE:
                     info = protocal.getServiceInfo(buffer);
                     provider.add(info);
                     Log.i(TAG, "register service info  "+  info.name+ ", ip:"+info.ip+", port:"+info.port+", info:"+info.info);
                     udp.send(protocal.getResultPkt(true, ""));
                     break;
                case Protocal.INFO_TYPE_UNREGISTER_SERVICE:
                     info = protocal.getServiceInfo(buffer);
                     boolean ret = provider.remove(info.dev, info.name);
                     Log.i(TAG, "unregister service info  "+  info.name+ ", ip:"+info.ip+", port:"+info.port+", info:"+info.info);
                     if(ret){
                         udp.send(protocal.getResultPkt(true, ""));
                     } else {
                         udp.send(protocal.getResultPkt(false, protocal.SERVICE_NO_FOUND));
                     }
                     break;
                case Protocal.INFO_TYPE_INQUIRY:
                     info = protocal.getServiceInfo(buffer);
                     Log.i(TAG, "inquiry service info  "+  info.name);
                     int count = provider.getCount(info.name);
                     if(count > 0){
                         for(int i=0; i<count; i++){
                             out_info = provider.get(info.name, i);
                             if(out_info!=null){
                                 udp.send(protocal.getServicePkt(out_info));
                             }
                         }
                     } else {
                         udp.send(protocal.getResultPkt(false, protocal.SERVICE_NO_FOUND));
                     }
                     break;
                default:
                     break;
                }
            }
        }
    }
    
    public void stop(){
        udp.exit();
    }

}