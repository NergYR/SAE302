import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.*;
import java.util.*;

public class CommServ {
    
    private int port = 5959;
    private String addr = "";
    private Socket socket = null;
    private DataInputStream input = null;
    private DataOutputStream output = null;

    public CommServ(String addr, int port){
        this.port = port;
        this.addr = addr;
        Connect();
    }

    public CommServ(String addr){
        this.addr = addr;
        Connect();
    }

    public void Connect(){
        try {
            this.socket = new Socket(this.addr,this.port);
            this.input = new DataInputStream(this.socket.getInputStream());
            this.output = new DataOutputStream(this.socket.getOutputStream());
        }
        catch (Exception e) {
            System.out.print(e.getMessage());
        }
    }

    public void Disconnect(){
        try{
            this.input.close();
            this.output.close();
            this.socket.close();
            System.out.print("Déconnecté");
        }
        catch(IOException e){
            System.out.print(e);
        }
    }

    public void GetInfo(){
        
    }

    public List<List<String>> GetFile(){
        String line = "";
        String[] temp = line.split("\n");
        List<String> lines = new ArrayList<>();
        for (int i = 0; i<temp.length;i++) {
            lines.add(temp[i]);
        }
        List<List<String>> csv_tab = new ArrayList<>();
        for(int i = 0; i<lines.size();i++){
            temp = lines.get(i).split(";");
            List<String>temp_list = new ArrayList<>();
            for(int j=0;j<temp.length;j++){
                temp_list.add(temp[j]);
            }
            csv_tab.add(temp_list);
        }
        return csv_tab;
    }

    public void SendFile(List<List<String>> file){
        String str_file = "";
        List<String> line = new ArrayList<>();
        for (int i=0;i<file.size();i++){
            line = file.get(i);
            for (int j=0;j<line.size();j++){
                str_file = str_file+file.get(j);
            }
            str_file += "\n";
        }
    }
}