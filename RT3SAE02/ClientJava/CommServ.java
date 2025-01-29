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
            //this.socket.connect(new InetSocketAddress(this.addr, this.port), this.port);
            this.input = new DataInputStream(this.socket.getInputStream());
            this.output = new DataOutputStream(this.socket.getOutputStream());
            //System.out.print(this.input.readUTF());
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
        try {
            String line = "";
            while ((line = this.input.readLine()).length()>=1){
                System.out.println(line);
            } /* */
        } 
        catch (Exception e) {
        }
    }

    public void SendInfo(String m){
        try {
            this.output.writeUTF(m);
        } catch (Exception e) {
        }
    }

    public List<List<String>> GetFile(){
        //transforme un string en liste de liste
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
        //transforme une liste de liste en string
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