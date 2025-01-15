import java.io.DataInputStream;
import java.io.DataOutputStream;
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

    public String Connect(){
        try {
            this.socket = new Socket(this.addr,this.port);
            this.input = new DataInputStream(this.socket.getInputStream());
            this.output = new DataOutputStream(this.socket.getOutputStream());
        }
        catch (Exception e) {
            System.out.print(e.getMessage());
        }
        return "";
    }

    public List<List<String>> GetInfo(){
        //reçu ligne par ligne
        String line = "";
        List<List<String>> csv_tab = new ArrayList<>();
        String[] tab_line = line.split(";");
        List<String> list_line = new ArrayList<>();
        for (String s : tab_line) {
            list_line.add(s);
        }
        csv_tab.add(list_line);
        return csv_tab;
    }

    public void SendFile(List<List<String>> file){
        //envoyer date puis fichier ligne par ligne
        List<String> list = null;
        String line = "";
        for (int i = 0; i < file.size(); i++) {
            list = file.get(i);
            for (int j = 0; j < list.size(); j++) {
                line += list.get(j)+";";
            }
        }
    }

    public void SendLine(){

    }
}
