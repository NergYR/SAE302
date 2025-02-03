
import java.util.List;
import java.util.Scanner;

public class ClientJava {

    private String cert = "";
    private String addr = "";
    private int port;

    public ClientJava(String cert){
        this.cert = cert;
    }

    public ClientJava(){

    }

    public void Start(){
        if(cert != ""){
            RunSSH();
        }
        else{
            Run();
        }
    }

    public void Run(){
        Scanner s = new Scanner(System.in);
        System.out.println("Entrez l'addresse du server");
        this.addr = s.nextLine();
        System.out.println("Entrez le port");
        this.port = Integer.parseInt(s.nextLine());
        CommServ cs = new CommServ(this.addr,this.port);
        List<String> name = cs.GetInfo();
        for(int i=0;i<name.size();i++){
            System.out.println(name.get(i));
        }
        System.out.println("Entrez le nom d'un fichier (avec son extention)");
        String file_name = s.nextLine();
        cs.SendInfo(file_name+"\n");
        List<List<String>> file = cs.GetFile();
        Presence p = new Presence(file);
        p.MarkPresence(p.Appel());
        cs.SendFile(p.getList());
        cs.Disconnect();
    }

    public void RunSSH(){
        
    }
}
