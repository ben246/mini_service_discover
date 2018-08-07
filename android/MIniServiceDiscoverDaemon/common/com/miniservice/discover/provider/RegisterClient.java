package com.miniservice.discover.provider;


import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

import android.content.Context;
import android.util.Log;

public class RegisterClient {
    private final static String TAG = "RegisterClient";
    UDPClient udp;
    Protocal protocal;
    Runnable r;
            
    boolean succ;
    String failReason;
    RegisterNoticeCallback notice;
    
    boolean recEnd;
    byte[] sendData;
    Thread thread;
    boolean runSearchFinish;
    
    public static interface RegisterNoticeCallback {
        public void RegisterNotice();
    }
    
    public RegisterClient(Context context){
        //udp = new UDPClient(context, true);
        udp = new UDPClient(context, false);
        protocal = new Protocal();
        runSearchFinish = true;
    }
    
    
    public void registerNotice(RegisterNoticeCallback cb){
        notice = cb;
    }

    public boolean registerSucc(){
        return succ;
    }
    
    
    public String getFailReason(){
        return failReason;
    }
    
    public void runRegister(ServiceInfo regInfo){
        if(!runSearchFinish){
            return;
        }
        runSearchFinish = false;
        if (udp.isBroadcast()){
            udp.send(protocal.getRegisterPkt(regInfo));
        } else {
            sendData = protocal.getRegisterPkt(regInfo);
            r = new Runnable() {
                @Override
                public void run() {
                    broadcastData(sendData);
                }
            };
            thread = new Thread(r);
            thread.start();
        }
        
        //Log.i(TAG, "send register service: "+regInfo.dev+regInfo.name+regInfo.ip);
        
        byte[] buffer = new byte[Protocal.PROTOCAL_LEN];
        ServiceInfo info;
        String reason;
        
        recEnd = false;
        while(true){
            int readLen = udp.receive(buffer);
            
            recEnd = true;
            
            if( readLen > 0 ){
                switch(protocal.decodeAction(buffer)){
                case Protocal.INFO_TYPE_ACTION_SUCC:
                     succ = true;
                     break;
                case Protocal.INFO_TYPE_ACTION_FAIL:
                     reason = protocal.getResult(buffer);
                     Log.i(TAG, "get service info fail, reason:"+ reason);
                     succ = false;
                     failReason = reason;
                     break;
                default:
                     break;
                }
            } else {
                break;
            }
        }
        
        if(notice!=null){
            notice.RegisterNotice();
        }
        
        if (!udp.isBroadcast()){
            try {
                thread.join();
                //Log.i(TAG, "runSearch join end");
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        
        runSearchFinish = true;
    }
    
    private void broadcastData(byte[] data) {
        boolean finish = false;
        udp.resetSubAddr();

        while (!finish) {
            if (recEnd) {
                break;
            }
            finish = udp.prepareSubAddr();
            udp.send(data);
        }

        //Log.i(TAG, "broadcastData quit");
    }
}