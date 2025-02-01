import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.*;

public class CommServ {
    
    private int port = 5959;
    private String addr = "";
    private Socket socket = null;
    private DataInputStream input = null;
    private DataOutputStream output = null;
    private BufferedReader in = null;

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
            in = new  BufferedReader(new InputStreamReader(this.socket.getInputStream()));
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

    public List<String> GetInfo(){
        List<String> result = new ArrayList<>();
        try {
            String line = ";";
            while (!line.trim().isEmpty() && line!=null){
                line = in.readLine();
                result.add(line);
            }
        } 
        catch (Exception e) {
        }
        return result;
    }

    public void SendInfo(String m){
        try {
            //this.output.write(m.getBytes());
            OutputStreamWriter writer = new OutputStreamWriter(this.output, StandardCharsets.UTF_8);
            writer.write(m + "\n"); // Ajoute un \n pour faciliter la lecture en C
            writer.flush(); // Envoi immédiat

        } catch (Exception e) {
        }
    }

    public List<List<String>> GetFile(){
        //transforme un string en liste de liste
        String line = "";
        String[] temp = null;
        List<List<String>> csv_tab = new ArrayList<>();
        try {
            List<String> lines = this.GetInfo();
            for(int i = 0; i<lines.size()-1;i++){
                temp = lines.get(i).split(";");
                List<String>temp_list = new ArrayList<>();
                for(int j=0;j<temp.length;j++){
                    temp_list.add(temp[j]);
                }
                csv_tab.add(temp_list);
            }
        } 
        catch (Exception e) {
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
                str_file = str_file+line.get(j)+";";
            }
            str_file += "\n";
        }
        SendInfo(str_file);
    }
}