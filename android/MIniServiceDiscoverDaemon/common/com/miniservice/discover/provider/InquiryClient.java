package com.miniservice.discover.provider;


import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

import android.content.Context;
import android.util.Log;

public class InquiryClient {
    private final static String TAG = "InquiryClient";
    UDPClient udp;
    Protocal protocal;
    Runnable r;

    boolean inquirySucc;
    String failReason;
    InquiryNoticeCallback notice;
    
    String serviceName;
    boolean recEnd;
    byte[] sendData;
    Thread thread;
    ServiceInfoList infoList;
    boolean runSearchFinish;
    
    public static interface InquiryNoticeCallback {
        public void InquiryNotice();
    }
    
    public InquiryClient(Context context){
        //udp = new UDPClient(context, true);
        udp = new UDPClient(context, false);
        protocal = new Protocal();
        infoList = new ServiceInfoList();
        runSearchFinish = true;
    }
    
    public void registerNotice(InquiryNoticeCallback cb){
        notice = cb;
    }

    public boolean inquirySucc(){
        return inquirySucc;
    }
    
    public ServiceInfoList getInquiryInfoList(){
        return infoList;
    }
    
    public String getFailReason(){
        return failReason;
    }
    
    public void runSearch(String serviceNameSet){
        if(!runSearchFinish){
            return;
        }
        runSearchFinish = false;
        infoList.clear();
        serviceName = serviceNameSet;
        if (udp.isBroadcast()){
            udp.send(protocal.getInquiryPkt(serviceName));
        } else {
            sendData = protocal.getInquiryPkt(serviceName);
            r = new Runnable() {
                @Override
                public void run() {
                    broadcastData(sendData);
                }
            };
            thread = new Thread(r);
            thread.start();
        }
        
        //Log.i(TAG, "send inquiry service:" + serviceName);
        
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
                     info = protocal.getServiceInfo(buffer);
                     //Log.i(TAG, "get service info  "+  info.name+ ", ip:"+info.ip+", port:"+info.port);
                     inquirySucc = true;
                     infoList.add(info);
                     break;
                case Protocal.INFO_TYPE_ACTION_FAIL:
                     reason = protocal.getResult(buffer);
                     Log.i(TAG, "get service info fail, reason:"+ reason);
                     inquirySucc = false;
                     failReason = reason;
                     break ;
                default:
                     break;
                }
            } else{
                break;
            }
        }
        
        if(notice!=null){
            notice.InquiryNotice();
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