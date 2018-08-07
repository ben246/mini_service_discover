package com.miniservice.discover.server.main;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import com.miniservice.discover.server.R;
import com.miniservice.discover.server.main.*;
import com.miniservice.discover.provider.*;

public class ServerActivity extends Activity implements RegisterClient.RegisterNoticeCallback {
    private static String TAG = "RegisterClientActivity";
    RegisterClient registerClient;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        
        registerClient = new RegisterClient(this);
        registerClient.registerNotice(this);
    }

    public void ButtonClick(View view){
        Log.i("button", "ButtonClick");
        
        final ServiceInfo info = new ServiceInfo("phone1", "play_music", "192.168.1.100", "play music with pcm format", 8090);
        
        Runnable r = new Runnable() {
            @Override
            public void run() {
                registerClient.runRegister(info);
            }
        };
        new Thread(r).start();
    }

    @Override
    public void RegisterNotice() {
        if(registerClient.registerSucc()){
             Log.i(TAG, "register service succ");
        } else {
             Log.i(TAG, "register service fail, reason:"+ registerClient.getFailReason());
        }
    }
}
