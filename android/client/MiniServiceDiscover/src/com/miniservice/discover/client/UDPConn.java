package com.miniservice.discover.client;


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
import java.net.SocketException;  
import java.util.Enumeration;  

public class UDPConn {
    private final static int DEFAULT_PORT = 9988;  
    private final static int UDP_TIMEOUT  = 500; //ms
    private final static String TAG = "UDPConn";
    
    private DatagramSocket datagramSocket;
    private DatagramPacket dataPacket;
    private InetAddress addr;
    
    private Context mContext;  
    
    public UDPConn(Context context){
        mContext = context;
        init();
    }
   
    public int receive(byte[] data){
        if(data.length != 0 &&
           addr != null) {  
            dataPacket = new DatagramPacket(data, data.length, addr, DEFAULT_PORT);
            try{
                datagramSocket.receive(dataPacket);
                return data.length;
            }catch(IOException e) {  
                //e.printStackTrace();  
            }  
        }  
        
        return 0;
    }
   
    public int send(byte[] data) {
        if(data.length != 0 &&
           addr != null) {  
            dataPacket = new DatagramPacket(data, data.length, addr, DEFAULT_PORT);
            try{
                datagramSocket.send(dataPacket);
            }catch(IOException e) {  
                //e.printStackTrace();  
            }  
        }  
        return 0;
    }
      
    private void init() {  
        try {  
            if(datagramSocket==null){  
                datagramSocket = new DatagramSocket(null);  
                datagramSocket.setReuseAddress(true);  
                datagramSocket.bind(new InetSocketAddress(DEFAULT_PORT)); 
                datagramSocket.setSoTimeout(UDP_TIMEOUT);
            }  
            
            String ip = getWIFIIP(mContext);
            if(ip!=""){
                ip = getBoradcastIP(ip);  
                Log.i(TAG,"ip="+ip);  
                
                addr = InetAddress.getByName(ip);
            } else {
                Log.e(TAG, "wifi ip is wrong, maybe you should link wifi first");
            }
        } catch(IOException e) {  
            e.printStackTrace();  
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
            return intToIp(ipAddress);  
        } else {
            return "";
        }
    }  
  
    private String intToIp(int i) {  
        return (( i & 0xFF)+ "."+ ((i >> 8 ) & 0xFF) + "."  
                + ((i >> 16 ) & 0xFF)  + "." +((i >> 24 ) & 0xFF ));  }  
}
