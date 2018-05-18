package com.miniservice.discover.main;

import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.util.Log;

import com.miniservice.discover.R;
import com.miniservice.discover.client.*;

import android.app.Activity;

public class MainActivity extends Activity {
    UDPClient udpClient;
   
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        udpClient = new UDPClient(this);
       
    }

    public void ButtonClick(View view){
        Log.i("button", "ButtonClick");
        udpClient.Run();
    }
}
