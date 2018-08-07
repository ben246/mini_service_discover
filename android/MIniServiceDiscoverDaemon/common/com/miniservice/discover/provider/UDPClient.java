package com.miniservice.discover.provider;


import android.content.Context;  
import android.net.wifi.WifiInfo;  
import android.net.wifi.WifiManager;  
import android.util.Log;  
  
import java.io.IOException;  
import java.net.DatagramPacket;  
import java.net.DatagramSocket;  
import java.net.InetAddress;  
import java.net.InetSocketAddress;  
import java.net.MulticastSocket;  
import java.net.NetworkInterface;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Enumeration;  

public class UDPClient {
    private final static int DEFAULT_PORT = 9988;  
    private final static int UDP_TIMEOUT  = 500; //ms
    private final static String TAG = "UDPClient";
    
    private DatagramSocket datagramSocket;
    private DatagramPacket dataPacket;
    private InetAddress broadcastAddr;
    private InetAddress subAddr;
    private Context mContext;  
    
    private String remoteAddr;
    private int remotePort;
    private String networkIP; //192.168.1.
    private int subIP;
    private boolean isBroadcast;
    
    public UDPClient(Context context, boolean isBroadcastSet){
        mContext = context;
        isBroadcast = isBroadcastSet;
        init();
    }
   
    public boolean isBroadcast() {
        return isBroadcast;
    }
    
    public int receive(byte[] data){
        if(data.length != 0) {  
            dataPacket = new DatagramPacket(data, data.length);
            dataPacket.setPort(DEFAULT_PORT);
            try{
                datagramSocket.receive(dataPacket);
                
                //Log.i(TAG, "remote addr:"+dataPacket.getAddress().toString()+"data len:"+ dataPacket.getLength()+"port:"+dataPacket.getPort());    
                return data.length;
            }catch(IOException e) {  
                //e.printStackTrace();  
            }  
        }  
        
        return 0;
    }
   
    public int send(byte[] data) {
        if(data.length != 0) {
            
            if(isBroadcast){
                dataPacket = new DatagramPacket(data, data.length, broadcastAddr, DEFAULT_PORT);
            } else {
                dataPacket = new DatagramPacket(data, data.length, subAddr, DEFAULT_PORT);
            }
            
            try{
                datagramSocket.send(dataPacket);
            }catch(IOException e) {  
                //e.printStackTrace();  
            }  
        }  
        return 0;
    }
    
    public void resetSubAddr() {
        subIP = 1;
    }

    public boolean prepareSubAddr(){
        boolean finish = false;
        
        String ip = networkIP + String.valueOf(subIP);
        //Log.i(TAG, "sub addr:" + ip);
        try{
            subAddr = InetAddress.getByName(ip);
        } catch(IOException e) {  
            e.printStackTrace();  
        }  
        if(subIP < 254) {
            subIP++;
        } else {
            finish = true;
        }
        return finish;
    }
    
    private void init() {  
        try {  
            datagramSocket = new DatagramSocket();  
            datagramSocket.setSoTimeout(UDP_TIMEOUT);
            
            String ip = getWIFIIP(mContext);
            if(ip!=""){
                Log.i(TAG,"ip="+ip);
                networkIP = getNetworkIP(ip);
                String bip = getBoradcastIP(ip);
                
                broadcastAddr = InetAddress.getByName(bip);
            } else {
                Log.e(TAG, "wifi ip is wrong, maybe you should link wifi first");
            }
        } catch(IOException e) {  
            e.printStackTrace();  
        }  
    }
    
    private String getNetworkIP(String IP){  
        int end = IP.lastIndexOf('.');
        if(end > 0){
            String temp=IP.substring(0,end+1);   
            StringBuffer buffer=new StringBuffer();  
            buffer.append(temp);
            return buffer.toString();  
        } else {
            return "";
        }
    }  
    
    private String getBoradcastIP(String IP){  
        int end = IP.lastIndexOf('.');
        if(end > 0){
            String temp=IP.substring(0,end+1);   
            StringBuffer buffer=new StringBuffer();  
            buffer.append(temp);  
            buffer.append("255");  
            return buffer.toString();  
        } else {
            return "";
        }
    }  
  
    private String getWIFIIP(Context context){  
        WifiManager wifiManager = (WifiManager)context.getSystemService(Context.WIFI_SERVICE);  
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();  
        int ipAddress = wifiInfo.getIpAddress();  
        
        if (ipAddress !=0 ){
            return intToIP(ipAddress);  
        } else {
            return "";
        }
    }  
  
    private String intToIP(int i) {  
        return (( i & 0xFF)+ "."+ ((i >> 8 ) & 0xFF) + "."  
                + ((i >> 16 ) & 0xFF)  + "." +((i >> 24 ) & 0xFF ));  }  
}
