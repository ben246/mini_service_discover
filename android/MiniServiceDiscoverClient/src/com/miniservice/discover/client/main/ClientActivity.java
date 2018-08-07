package com.miniservice.discover.client.main;

import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.util.Log;

import com.miniservice.discover.client.R;
import android.app.Activity;
import com.miniservice.discover.provider.*;


public class ClientActivity extends Activity implements InquiryClient.InquiryNoticeCallback {
    
    private static String TAG = "DiscoverClientActivity";
    InquiryClient discoverClient;
   
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        discoverClient = new InquiryClient(this);
        discoverClient.registerNotice(this);
    }

    public void ButtonClick(View view){
        Log.i("button", "ButtonClick");
        
        Runnable r = new Runnable() {
            @Override
            public void run() {
                discoverClient.runSearch("play_music");
            }
        };
        new Thread(r).start();
    }

    @Override
    public void InquiryNotice() {
        if(discoverClient.inquirySucc()){
             ServiceInfoList list = discoverClient.getInquiryInfoList();
             for(ServiceInfo info: list.getList()){
                 Log.i(TAG, "get service info:"+ "dev:" + info.dev +"," +  "name:" + info.name+ ", ip:"+info.ip+", port:"+info.port+", info:"+info.info);
             }
        } else {
             Log.i(TAG, "get service info fail, reason:"+ discoverClient.getFailReason());
        }
    }
}