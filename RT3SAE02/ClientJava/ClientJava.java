
import java.util.*;

public class ClientJava {

    private String cert = "";
    private String addr = "";

    public ClientJava(String cert, String addr){
        this.cert = cert;
        this.addr = addr;
    }

    public ClientJava(String addr){
        this.addr = addr;
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
        CommServ cs = new CommServ(this.addr);
        List<String> name = cs.GetInfo();
        for(int i=0;i<name.size();i++){
            System.out.println(name.get(i));
        }
        Scanner s = new Scanner(System.in);
        System.out.println("Entrez le nom d'un fichier (avec son extention)");
        String file_name = s.nextLine();
        cs.SendInfo(file_name+"\n");
        List<List<String>> file = cs.GetFile();
        Presence p = new Presence(file);
        p.Console_MarkPresenceStudent(p.Appel());
        cs.SendFile(p.getList());
        cs.Disconnect();
    }

    public void RunSSH(){
        
    }
}
