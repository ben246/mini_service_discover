package com.miniservice.discover.client;


import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

import android.content.Context;
import android.util.Log;

public class UDPClient {
    private final static String TAG = "UDPClient";
    UDPConn udp;
    Protocal protocal;
    Runnable r;

    public UDPClient(Context context){
        udp = new UDPConn(context);
        protocal = new Protocal();
    }
    
    public void Run(){
         r = new Runnable() {
            @Override
            public void run() {
                //testLoop();
                loop();
            }
        };
        new Thread(r).start();
    }

    private void loop(){
        udp.send(protocal.getInquiryPkt("play_music"));
        
        
        byte[] buffer = new byte[Protocal.PROTOCAL_LEN];
        
        while(true){
            ServiceInfo info;
            String reason;
            if( udp.receive(buffer) > 0){
                switch(protocal.decodeAction(buffer)){
                case Protocal.INFO_TYPE_ACTION_SUCC:
                     info = protocal.getServiceInfo(buffer);
                     Log.i(TAG, "get service info  "+  info.name+ ", ip:"+info.ip+", port:"+info.port);
                     break;
                case Protocal.INFO_TYPE_ACTION_FAIL:
                     reason = protocal.getResult(buffer);
                     Log.i(TAG, "get service info fail, reason:"+ reason);
                     break ;
                default:
                     break;
                }
            }
        }
    }
    
    private void testLoop(){
        byte[] buffer = new byte[256];
        int port = 9988;
        DatagramSocket server = null;
        try {
            Log.i("udp ready",  "start read");
            server = new DatagramSocket (port);
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
            while(true){
                try {
                    server.receive(packet);
                    String s = new String(packet.getData(), 0, packet.getLength(), "UTF-8");
                    Log.i("udp rec", "address : " + packet.getAddress() + ", port : " + packet.getPort() + ", content : " + s);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        } catch (SocketException e) {
            e.printStackTrace();
        }finally{
            if(server != null)
                server.close();
        }
    }
}