package com.miniservice.discover.daemon;

import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.util.Log;
import android.app.Activity;
import android.content.Context;
import android.net.wifi.WifiManager;

import com.miniservice.discover.daemon.R;
import com.miniservice.discover.provider.*;

public class DaemonActivity extends Activity {
    private static String TAG = "DiscoverServerActivity";
    //private DiscoverServer discoverServer;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        //discoverServer = new DiscoverServer(this);
    }

    public void ButtonClick(View view){
        Log.i("button", "ButtonClick");
        //discoverServer.run();
    }
    
    @Override
    protected void onDestroy(){
        super.onDestroy();
        //discoverServer.stop();
    }
}