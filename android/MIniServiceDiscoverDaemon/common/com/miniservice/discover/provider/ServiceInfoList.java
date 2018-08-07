package com.miniservice.discover.provider;

import java.util.LinkedList;

import android.util.Log;

public class ServiceInfoList {
    private static final String TAG = "Provider";
    LinkedList<ServiceInfo> list;
    
    public ServiceInfoList(){
        list = new LinkedList<ServiceInfo>();
    }
    
    public LinkedList<ServiceInfo> getList() {
        return list;
    }
    
    public void add(ServiceInfo info){
        remove(info.dev, info.name);
        list.add(info);
    }
    
    public void clear() {
        for(ServiceInfo info: list){
            list.remove(info);
        }
    }
    
    public boolean remove(String devName, String serviceName){
        for(ServiceInfo info: list){
            if(info.dev.equals(devName)  && info.name.equals(serviceName)){
                list.remove(info);
                return true;
            }
        }
        return false;
    }
    
    public int getCount(String serviceName){
        int i = 0;
        for(ServiceInfo info: list){
            Log.i(TAG,  info.name);
            if(info.name.equals(serviceName)){
                i++;
            }
        }
        
        return i;
    }
    
    public ServiceInfo get(String serviceName, int index){
        int i = 0;
        for(ServiceInfo info: list){
            if(info.name.equals(serviceName)  && i==index){
                return info;
            }
            i++;
        }
        
        return null;
    }
}
