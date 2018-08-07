package com.miniservice.discover.client;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.HashMap;
import java.util.Map;

import org.json.JSONException;
import org.json.JSONObject;
import android.util.Log;

public class Protocal {
    private final static String TAG = "Protocal";
    public final static int PROTOCAL_LEN = 1024;
    public final static short INFO_HEAD = (short)0xFFEE;
    public final static short INFO_TYPE_REGISTER_SERVICE   = 0;
    public final static short INFO_TYPE_UNREGISTER_SERVICE = 1;
    public final static short INFO_TYPE_INQUIRY            = 2;
    public final static short INFO_TYPE_ACTION_SUCC        = 3;
    public final static short INFO_TYPE_ACTION_FAIL        = 4;
    public final static short INFO_TYPE_ACTION_INVALID     = -1;
   

    public static class protocalHead{
        public short head;
        public short action;
        public short len;
        public short checksum;
        
        private short getShort(byte[] data, int offset){
            return (short) (((data[offset + 1] << 8) | data[offset + 0] & 0xff));  
        }
        
        public protocalHead() {}
        
        public protocalHead(byte[] data) {
            head =  getShort(data, 0);
            action =  getShort(data, 2);
            len =  getShort(data, 4);
            checksum = getShort(data, 6);
        }
        
        public int getSize(){
            return Short.SIZE/8*4;
        }
        
        public byte[] getBytes(){
            byte[] data = new byte[getSize()];
            int i = 0;
            
            data[i++] = (byte)(head&0xFF);
            data[i++] = (byte)((head>>8)&0xFF);
            data[i++] = (byte)(action&0xFF);
            data[i++] = (byte)((action>>8)&0xFF);
            data[i++] = (byte)(len&0xFF);
            data[i++] = (byte)((len>>8)&0xFF);
            data[i++] = (byte)(checksum&0xFF);
            data[i++] = (byte)((checksum>>8)&0xFF);
            return data;
        }
    };
    
    private short getChecksumByOffset(byte[] data, int offset, int len){
        short sum = 0;
        //Log.i(TAG, new String(data, offset, len));
        for(int i=0; i<len; i++){
            sum +=data[offset+i];
        }
        return sum;
    }
    
    private short getChecksum(byte[] data){
        return getChecksumByOffset(data, 0, data.length);
    }
    
    public byte[] getInquiryPkt(String serviceName){
        byte[] data = new byte[PROTOCAL_LEN];
        
        protocalHead head = new protocalHead();
        head.head = INFO_HEAD;
        head.action = INFO_TYPE_INQUIRY;
        
        Map map = new HashMap();
        map.put("device_name", "");
        map.put("service_name", serviceName);
        map.put("ip", "");
        map.put("port", 0);
        
        JSONObject json = new JSONObject(map);
        byte[] jsonData = json.toString().getBytes();
        
        head.len = (short)jsonData.length;
        head.checksum = getChecksum(jsonData);
        
        //Log.i(TAG, "getInquiryPkt json:"+jsonData.toString()+" len="+jsonData.toString().length()+" checksum:"+head.checksum);
        
        System.arraycopy(head.getBytes(), 0, data, 0,  head.getSize());
        System.arraycopy(jsonData,        0, data, head.getSize(),  jsonData.length);
        return data;
    }
    
    
    public int decodeAction(byte[] data) {
        protocalHead  head = new protocalHead(data);
        //Log.i(TAG, "head"+head.head);
        //Log.i(TAG, "action"+head.action);
        //Log.i(TAG, "len"+head.len);
        //Log.i(TAG, "checksum"+head.checksum);
        short checksum = getChecksumByOffset(data, head.getSize(), head.len);
        if(checksum == head.checksum){
            return head.action;
        } else {
            Log.e(TAG, "checksum fail, cal="+checksum+" rec="+head.checksum);
            return INFO_TYPE_ACTION_INVALID;
        }
    }
    
    public ServiceInfo getServiceInfo(byte[] data){
        ServiceInfo info = new ServiceInfo();
        protocalHead  head = new protocalHead(data);

        try {
            String json = new String(data, head.getSize(), head.len, "UTF-8");
            //Log.i(TAG, "getServiceInfo json:"+json);
            try{
                JSONObject jsonObj = new JSONObject(json);
                info.dev = jsonObj.getString("device_name");
                info.name = jsonObj.getString("service_name");
                info.ip = jsonObj.getString("ip");
                info.port = jsonObj.getInt("port");
            }catch(JSONException e) {  
                e.printStackTrace();  
            }  
        } catch (UnsupportedEncodingException e1) {
            e1.printStackTrace();
        }
       
        return info;
    }
    
    public String getResult(byte[] data){
        protocalHead  head = new protocalHead(data);
        
        byte[] jsonData = new byte[head.len];
        System.arraycopy(data, head.getSize(), jsonData, 0,  head.len);
        
        try {
            String json = new String(data, head.getSize(), head.len, "UTF-8");
            try{
                JSONObject jsonObj = new JSONObject(json);
                return jsonObj.getString("reason");
            }catch(JSONException e) {  
                e.printStackTrace();  
            }  
        } catch (UnsupportedEncodingException e1) {
            e1.printStackTrace();
        }
        return "";
    }
}