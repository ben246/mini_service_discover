package com.miniservice.discover.daemon;



import android.app.Application;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.IBinder;
import android.util.Log;
import com.miniservice.discover.provider.*;

public class DaemonApp extends Application {
    private static final String TAG = "DaemonApp";
    private ProviderServer providerServer;
    
    @Override  
    public void onCreate(){  
        super.onCreate();  
        Log.d(TAG,  "onCreate");
        
        providerServer = new ProviderServer(this);
        providerServer.run();
    }  

    
    @Override  
    public void onTerminate(){
        super.onTerminate();
        Log.d(TAG,  "onTerminate");
        providerServer.stop();
    }
    
    public static class DaemonAppReceiver extends BroadcastReceiver{  
        private static final String TAG = "DaemonAppBoot";
        
        public DaemonAppReceiver(){
            super();
        }
        
        @Override  
        public void onReceive(Context context, Intent intent) {  
            if(intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED)){
                Log.d(TAG,  "onBootCompletedReceive");
                
            }
        }  
    }  
    
}