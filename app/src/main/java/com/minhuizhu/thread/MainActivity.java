package com.minhuizhu.thread;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private EditText threadsEdit;
    private EditText iterationsEdit;
    private Button startButton;
    private TextView logView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        nativeInit();

        threadsEdit = (EditText) findViewById(R.id.threads_edit);
        iterationsEdit = (EditText) findViewById(R.id.iterations_edit);
        startButton = (Button) findViewById(R.id.start_button);
        logView = (TextView) findViewById(R.id.log_view);

        startButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                int threads = getNumber(threadsEdit, 0);
                int iterations = getNumber(iterationsEdit, 0);

                if (threads > 0 && iterations > 0) {
                    startThreads(threads, iterations);
                }
            }
        });
    }
    private void javaThreads(int threads,final int iterations){
        for(int i=0;i<threads;i++){
            final int id=i;
            Thread thread=new Thread(){
                @Override
                public void run() {
                    nativeWorker(id,iterations);
                }
            };
            thread.start();
        }
    }
    private native void nativeFree();
    private native void nativeWorker(int id, int iterations);

    private void startThreads(int threads, int iterations) {
        posixThreads(threads,iterations);
    }



    private int getNumber(EditText editText, int defaultValue) {
        int num;
        try {
            num=Integer.parseInt(editText.getText().toString().trim());
        }catch (Exception e){
            num=defaultValue;
        }
        return num;
    }

    private native void  nativeInit() ;
    private native void posixThreads(int threads, int iterations);
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    private void onNativeMessage(final String message){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                logView.append(message);
                logView.append("\n");
            }
        });
    }

    @Override
    protected void onDestroy() {
        nativeFree();
        super.onDestroy();
    }
}















