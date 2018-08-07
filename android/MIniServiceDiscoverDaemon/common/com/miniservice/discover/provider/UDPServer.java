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

public class UDPServer {
    private final static int DEFAULT_PORT = 9988;  
    private final static String TAG = "UDPServer";
    
    private DatagramSocket datagramSocket;
    private DatagramPacket dataPacket;

    private InetAddress raddr;
    private int         rport;
    
    private Context mContext;
    
    
    public UDPServer(Context context){
        mContext = context;
        init();
    }
   
    public int receive(byte[] data){
        if(data.length != 0) {  
            dataPacket = new DatagramPacket(data, data.length);
            try{
                datagramSocket.receive(dataPacket);
                
                raddr = dataPacket.getAddress();
                rport = dataPacket.getPort();
                Log.i(TAG, "remote addr:"+dataPacket.getAddress().toString()+"data len:"+ dataPacket.getLength()+"port:"+dataPacket.getPort());
                
                return data.length;
            }catch(IOException e) {  
                //e.printStackTrace();  
            }  
        }  
        
        return 0;
    }
   
   
    public int send(byte[] data) {
        if(data.length != 0 &&
            raddr != null) {  
            
            dataPacket = new DatagramPacket(data, data.length, raddr, rport);  
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
            datagramSocket = new DatagramSocket(DEFAULT_PORT);  
        } catch(IOException e) {  
            e.printStackTrace();  
        }  
    }
    
    public void exit(){
        if(datagramSocket != null ){
            datagramSocket.close();
        }
    }
    
}
