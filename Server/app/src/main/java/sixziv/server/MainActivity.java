//Refering to https://examples.javacodegeeks.com/android/core/socket-core/android-socket-example/
//Generated on 2018/7/20

package sixziv.server;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;

import android.app.Activity;
import android.os.Handler;
import android.telephony.SmsManager;
import android.text.method.ScrollingMovementMethod;
import android.view.WindowManager;
import android.widget.TextView;

import org.json.JSONException;
import org.json.JSONObject;

public class MainActivity extends Activity {
    private ServerSocket serverSocket;
    Handler updateConversationHandler;
    Thread serverThread = null;
    private TextView text;
    static final int PORT = 6000;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        text = findViewById(R.id.text2);
        text.setMovementMethod(ScrollingMovementMethod.getInstance());
        updateConversationHandler = new Handler();

        this.serverThread = new Thread(new ServerThread());
        this.serverThread.start();
    }

    @Override
    protected void onStop() {
        super.onStop();
        try {
            serverSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    class ServerThread implements Runnable {
        public void run() {
            try {
                serverSocket = new ServerSocket(PORT);
            } catch (IOException e) {
                e.printStackTrace();
            }
            while (!Thread.currentThread().isInterrupted()) {
                try {
                    Socket socket = serverSocket.accept();
                    CommunicationThread commThread = new CommunicationThread(socket);
                    new Thread(commThread).start();
                    updateConversationHandler.post(new updateUIThread("Server","Connected!"));
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    class CommunicationThread implements Runnable {

        private Socket clientSocket;
        private BufferedReader input;
        private BufferedWriter output;
        private CommunicationThread(Socket clientSocket) {
            this.clientSocket = clientSocket;
            try {
                this.input = new BufferedReader(new InputStreamReader(this.clientSocket.getInputStream()));
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        public void run() {
            while (!Thread.currentThread().isInterrupted()) {
                try {
                    String read = input.readLine();
                    if(read.isEmpty())continue;
                    if(read.trim().isEmpty())continue;
                    if(read.trim().equals("ping")) {
                        output = new BufferedWriter(new OutputStreamWriter(this.clientSocket.getOutputStream()));
                        output.write("ping");
                        output.flush();
                    }
                    try {
                        JSONObject obj = new JSONObject(read);
                        String p_num=obj.getString("phone").trim();
                        String msg=obj.getString("msg").trim();
                        SmsManager smsManager = SmsManager.getDefault();
                        smsManager.sendTextMessage(p_num, null, msg,null ,null );
                        output = new BufferedWriter(new OutputStreamWriter(this.clientSocket.getOutputStream()));
                        output.write("done");
                        output.flush();
                        updateConversationHandler.post(new updateUIThread(p_num,msg));
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    class updateUIThread implements Runnable {
        private String msg;
        private String p_num;
        private updateUIThread(String par_p_num,String par_msg) {
            this.p_num = par_p_num;
            this.msg = par_msg;

        }

        @Override
        public void run() {
            if(p_num.equals("Server"))
                text.append(msg +"\n");
            else
                text.append("Sending Message To " +p_num+"\n");
            int offset=text.getLineCount()*text.getLineHeight();
            if(offset>(text.getHeight()-text.getLineHeight()-20)){
                text.scrollTo(0,offset-text.getHeight()+text.getLineHeight()+20);
            }
        }
    }
}